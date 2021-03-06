/**
GDevelop - Platform Behavior Extension
Copyright (c) 2013-2015 Florian Rival (Florian.Rival@gmail.com)
 */

/**
 * PlatformerObjectRuntimeBehavior represents a behavior allowing objects to be
 * considered as a platform by objects having PlatformerObject Behavior.
 *
 * @class PlatformerObjectRuntimeBehavior
 * @constructor
 */
gdjs.PlatformerObjectRuntimeBehavior = function(runtimeScene, behaviorData, owner)
{
    gdjs.RuntimeBehavior.call(this, runtimeScene, behaviorData, owner);

    this._gravity = behaviorData.gravity;
    this._maxFallingSpeed = behaviorData.maxFallingSpeed;
    this._acceleration = behaviorData.acceleration;
    this._deceleration = behaviorData.deceleration;
    this._maxSpeed = behaviorData.maxSpeed;
    this._jumpSpeed = behaviorData.jumpSpeed;
    this._isOnFloor = false;
    this._isOnLadder = false;
    this._floorPlatform = null;
    this._currentFallSpeed = 0;
    this._currentSpeed = 0;
    this._jumping = false;
    this._currentJumpSpeed = 0;
    this._canJump = false;
    this._ignoreDefaultControls = behaviorData.ignoreDefaultControls;
    this._leftKey = false;
    this._rightKey = false;
    this._ladderKey = false;
    this._upKey = false;
    this._downKey = false;
    this._jumpKey = false;
    this._potentialCollidingObjects = []; //Platforms near the object, updated with _updatePotentialCollidingObjects.
    this._overlappedJumpThru =[];
    this._oldHeight = 0;//owner.getHeight(); //Be careful, object might not be initialized.
    this._hasReallyMoved = false;
    this.setSlopeMaxAngle(behaviorData.slopeMaxAngle);
    this._manager = gdjs.PlatformObjectsManager.getManager(runtimeScene);
};

gdjs.PlatformerObjectRuntimeBehavior.prototype = Object.create( gdjs.RuntimeBehavior.prototype );
gdjs.PlatformerObjectRuntimeBehavior.thisIsARuntimeBehaviorConstructor = "PlatformBehavior::PlatformerObjectBehavior";

gdjs.PlatformerObjectRuntimeBehavior.prototype.doStepPreEvents = function(runtimeScene)
{
    var LEFTKEY = 37;
    var UPKEY = 38;
    var RIGHTKEY = 39;
    var DOWNKEY = 40;
    var SHIFTKEY = 16;
    var SPACEKEY = 32;
    var object = this.owner;
    var timeDelta = runtimeScene.getTimeManager().getElapsedTime()/1000;

    //0.1) Get the player input:
    var requestedDeltaX = 0;
    var requestedDeltaY = 0;

    //Change the speed according to the player's input.
    this._leftKey |= !this._ignoreDefaultControls && runtimeScene.getGame().getInputManager().isKeyPressed(LEFTKEY);
    this._rightKey |= !this._ignoreDefaultControls && runtimeScene.getGame().getInputManager().isKeyPressed(RIGHTKEY);
    if ( this._leftKey ) this._currentSpeed -= this._acceleration*timeDelta;
    if ( this._rightKey ) this._currentSpeed += this._acceleration*timeDelta;

    //Take deceleration into account only if no key is pressed.
    if ( this._leftKey == this._rightKey ) {
        var wasPositive = this._currentSpeed>0;
        this._currentSpeed -= this._deceleration*timeDelta*(wasPositive ? 1.0 : -1.0);

        //Set the speed to 0 if the speed was top low.
        if ( wasPositive && this._currentSpeed < 0 ) this._currentSpeed = 0;
        if ( !wasPositive && this._currentSpeed > 0 ) this._currentSpeed = 0;
    }

    if (this._currentSpeed > this._maxSpeed) this._currentSpeed = this._maxSpeed;
    if (this._currentSpeed < -this._maxSpeed) this._currentSpeed = -this._maxSpeed;
    requestedDeltaX += this._currentSpeed*timeDelta;

    //Compute the list of the objects that will be used
    this._updatePotentialCollidingObjects(Math.max(requestedDeltaX, this._maxFallingSpeed) );
    this._updateOverlappedJumpThru();

    //Check that the floor object still exists and is near the object.
    if ( this._isOnFloor && !this._isIn(this._potentialCollidingObjects, this._floorPlatform.owner.id) ) {
        this._isOnFloor = false;
        this._floorPlatform = null;
    }

    //0.2) Track changes in object size

    //Stick the object to the floor if its height has changed.
    if ( this._isOnFloor && this._oldHeight !== object.getHeight() )
        object.setY(object.getY()+this._oldHeight-object.getHeight());

    this._oldHeight = object.getHeight();

    //1) X axis:

    //Shift the object according to the floor movement.
    if ( this._isOnFloor ) {
        requestedDeltaX += this._floorPlatform.owner.getX() - this._floorLastX;
        requestedDeltaY += this._floorPlatform.owner.getY() - this._floorLastY;
    }

    //Ensure the object is not stuck
    if (this._separateFromPlatforms(this._potentialCollidingObjects, true)) {
        this._canJump = true; //After being unstuck, the object must be able to jump again.
    }

    //Move the object on x axis.
    var oldX = object.getX();
    if ( requestedDeltaX !== 0 ) {

        var floorPlatformId = this._floorPlatform !== null ? this._floorPlatform.owner.id : null;
        object.setX(object.getX()+requestedDeltaX);
        //Colliding: Try to push out from the solid.
        //Note that jump thru are never obstacle on X axis.
        while ( this._isCollidingWith(this._potentialCollidingObjects, floorPlatformId, /*excludeJumpthrus=*/true) ) {
            if ( (requestedDeltaX > 0 && object.getX() <= oldX) ||
                 (requestedDeltaX < 0 && object.getX() >= oldX) ) {
                object.setX(oldX); //Unable to move the object without being stuck in an obstacle.
                break;
            }

            //If on floor: try get up a bit to bypass not perfectly aligned floors.
            if ( this._isOnFloor ){
                object.setY(object.getY()-1);
                if ( !this._isCollidingWith(this._potentialCollidingObjects, floorPlatformId, /*excludeJumpthrus=*/true) )
                    break;
                object.setY(object.getY()+1);
            }

            object.setX(Math.floor(object.getX())+(requestedDeltaX > 0 ? -1 : 1));
            this._currentSpeed = 0; //Collided with a wall
        }
    }

    //2) Y axis:

    //Go on a ladder
    this._ladderKey |= !this._ignoreDefaultControls && runtimeScene.getGame().getInputManager().isKeyPressed(UPKEY);
    if (this._ladderKey && this._isOverlappingLadder()) {
        this._canJump = true;
        this._isOnFloor = false;
        this._floorPlatform = null;
        this._currentJumpSpeed = 0;
        this._currentFallSpeed = 0;
        this._isOnLadder = true;
    }

    if ( this._isOnLadder ) {
        this._upKey |= !this._ignoreDefaultControls && runtimeScene.getGame().getInputManager().isKeyPressed(UPKEY);
        this._downKey |= !this._ignoreDefaultControls && runtimeScene.getGame().getInputManager().isKeyPressed(DOWNKEY);
        if ( this._upKey )
            requestedDeltaY -= 150*timeDelta;
        if ( this._downKey )
            requestedDeltaY += 150*timeDelta;

        //Coming to an extremity of a ladder
        if ( !this._isOverlappingLadder() ) {
            this._isOnLadder = false;
        }
    }

    //Fall
    if (!this._isOnFloor && !this._isOnLadder) {
        this._currentFallSpeed += this._gravity*timeDelta;
        if ( this._currentFallSpeed > this._maxFallingSpeed ) this._currentFallSpeed = this._maxFallingSpeed;

        requestedDeltaY += this._currentFallSpeed*timeDelta;
        requestedDeltaY = Math.min(requestedDeltaY, this._maxFallingSpeed*timeDelta);
    }

    //Jumping
    this._jumpKey |= !this._ignoreDefaultControls &&
        (runtimeScene.getGame().getInputManager().isKeyPressed(SHIFTKEY) ||
        runtimeScene.getGame().getInputManager().isKeyPressed(SPACEKEY));
    if ( this._canJump && this._jumpKey ) {
        this._jumping = true;
        this._canJump = false;
        //this._isOnFloor = false; If floor is a very steep slope, the object could go into it.
        this._isOnLadder = false;
        this._currentJumpSpeed = this._jumpSpeed;
        this._currentFallSpeed = 0;
        //object.setY(object.getY()-1); //Useless and dangerous
    }

    if ( this._jumping ) {
        requestedDeltaY -= this._currentJumpSpeed*timeDelta;
        this._currentJumpSpeed -= this._gravity*timeDelta;
        if ( this._currentJumpSpeed < 0 ) {
            this._currentJumpSpeed = 0;
            this._jumping = false;
        }
    }

    //Follow the floor
    if ( this._isOnFloor ) {
        if ( gdjs.RuntimeObject.collisionTest(object, this._floorPlatform.owner) ) {
            //Floor is getting up, as the object is colliding with it.
            var oldY = object.getY();
            var step = 0;
            var stillInFloor = false;
            do {
                if ( step >= Math.floor(Math.abs(requestedDeltaX*this._slopeClimbingFactor)) ) { //Slope is too step ( > 50% )

                    object.setY(object.getY()-(Math.abs(requestedDeltaX*this._slopeClimbingFactor)-step)); //Try to add the decimal part.
                    if ( gdjs.RuntimeObject.collisionTest(object, this._floorPlatform.owner) )
                        stillInFloor = true; //Too steep.

                    break;
                }

                //Try to get out of the floor.
                object.setY(object.getY()-1);
                step++;
            }
            while ( gdjs.RuntimeObject.collisionTest(object, this._floorPlatform.owner ) );

            if ( stillInFloor ) {
                object.setY(oldY); //Unable to follow the floor ( too steep ): Go back to the original position.
                object.setX(oldX); //And also revert the shift on X axis.
            }
        }
        else {
            //Floor is flat or get down.
            var oldY = object.getY();
            object.setY(object.getY()+1);
            var step = 0;
            var noMoreOnFloor = false;
            while ( !this._isCollidingWith(this._potentialCollidingObjects) ) {
                if ( step > Math.abs(requestedDeltaX*this._slopeClimbingFactor) ) { //Slope is too step ( > 50% )
                    noMoreOnFloor = true;
                    break;
                }

                //Object was on floor, but no more: Maybe a slope, try to follow it.
                object.setY(object.getY()+1);
                step++;
            }
            if ( noMoreOnFloor )
                object.setY(oldY); //Unable to follow the floor: Go back to the original position. Fall will be triggered next tick.
            else
                object.setY(object.getY()-1); //Floor touched: Go back 1 pixel over.
        }

    }

    //Move the object on Y axis
    if ( requestedDeltaY !== 0 ) {
        var oldY = object.getY();
        object.setY(object.getY()+requestedDeltaY);

        //Stop when colliding with an obstacle.
        while (  (requestedDeltaY < 0 && this._isCollidingWith(this._potentialCollidingObjects, null, /*excludeJumpThrus=*/true)) //Jumpthru = obstacle <=> Never when going up
              || (requestedDeltaY > 0 && this._isCollidingWithExcluding(this._potentialCollidingObjects, this._overlappedJumpThru)) ) //Jumpthru = obstacle <=> Only if not already overlapped when goign down
        {
            this._jumping = false;
            this._currentJumpSpeed = 0;
            if ( (requestedDeltaY > 0 && object.getY() <= oldY) ||
                 (requestedDeltaY < 0 && object.getY() >= oldY) ) {
                object.setY(oldY); //Unable to move the object without being stuck in an obstacle.
                break;
            }

            object.setY(Math.floor(object.getY())+(requestedDeltaY > 0 ? -1 : 1));
        }

    }

    //3) Update the current floor data for the next tick:
    this._updateOverlappedJumpThru();
    if ( !this._isOnLadder ) {
        //Check if the object is on a floor:
        //In priority, check if the last floor platform is still the floor.
        var oldY = object.getY();
        object.setY(object.getY()+1);
        if ( this._isOnFloor && gdjs.RuntimeObject.collisionTest(object, this._floorPlatform.owner ) ) {
            //Still on the same floor
            this._floorLastX = this._floorPlatform.owner.getX();
            this._floorLastY = this._floorPlatform.owner.getY();
        }
        else{
            //Check if landing on a new floor: (Exclude already overlapped jump truh)
            var collidingPlatform = this._getCollidingPlatform();
            if (collidingPlatform !== null) {
                this._isOnFloor = true;
                this._canJump = true;
                this._jumping = false;
                this._currentJumpSpeed = 0;
                this._currentFallSpeed = 0;

                //Register the colliding platform as the floor.
                this._floorPlatform = collidingPlatform;
                this._floorLastX = this._floorPlatform.owner.getX();
                this._floorLastY = this._floorPlatform.owner.getY();
            } else { //In the air
                this._canJump = false;
                this._isOnFloor = false;
                this._floorPlatform = null;
            }
        }
        object.setY(oldY);
    }

    //4) Do not forget to reset pressed keys
    this._leftKey = false;
    this._rightKey = false;
    this._ladderKey = false;
    this._upKey = false;
    this._downKey = false;
    this._jumpKey = false;

    //5) Track the movement
    this._hasReallyMoved = Math.abs(object.getX()-oldX) >= 1;
};

gdjs.PlatformerObjectRuntimeBehavior.prototype.doStepPostEvents = function(runtimeScene) {
    //Scene change is not supported
    /*
    if ( parentScene != &scene ) //Parent scene has changed
    {
        parentScene = &scene;
        sceneManager = parentScene ? &ScenePlatformObjectsManager::managers[&scene] : null;
        floorPlatform = null;
    }
    */
};

/**
 * Among the platforms passed in parameter, return true if there is a platform colliding with the object.
 * Ladders are *always* excluded from the test.
 * @param candidates The platform to be tested for collision
 * @param exceptThisOne The object identifier of a platform to be excluded from the check. Can be null.
 * @param excludeJumpThrus If set to true, jumpthru platforms are excluded. false if not defined.
 */
gdjs.PlatformerObjectRuntimeBehavior.prototype._isCollidingWith = function(candidates, exceptThisOne, excludeJumpThrus)
{
    excludeJumpThrus = !!excludeJumpThrus;

    for (var i = 0;i<candidates.length;++i) {
        var platform = candidates[i];

        if ( platform.owner.id === exceptThisOne ) continue;
        if ( platform.getPlatformType() === gdjs.PlatformRuntimeBehavior.LADDER ) continue;
        if ( excludeJumpThrus && platform.getPlatformType() === gdjs.PlatformRuntimeBehavior.JUMPTHRU ) continue;

        if ( gdjs.RuntimeObject.collisionTest(this.owner, platform.owner) )
            return true;
    }

    return false;
};

/**
 * Separate the object from all platforms passed in parameter.
 * @param candidates The platform to be tested for collision
 * @param excludeJumpThrus If set to true, jumpthru platforms are excluded. false if not defined.
 */
gdjs.PlatformerObjectRuntimeBehavior.prototype._separateFromPlatforms = function(candidates, excludeJumpThrus)
{
    excludeJumpThrus = !!excludeJumpThrus;

    var objects = [];
    for (var i = 0;i<candidates.length;++i) {
        var platform = candidates[i];

        if ( platform.getPlatformType() === gdjs.PlatformRuntimeBehavior.LADDER ) continue;
        if ( excludeJumpThrus && platform.getPlatformType() === gdjs.PlatformRuntimeBehavior.JUMPTHRU ) continue;

        objects.push(platform.owner);
    }

    var objectsLists = new Hashtable();
    objectsLists.put("", objects);
    return this.owner.separateFromObjects(objectsLists);
};

/**
 * Among the platforms passed in parameter, return true if there is a platform colliding with the object.
 * Ladders are *always* excluded from the test.
 * @param candidates The platform to be tested for collision
 * @param exceptTheseOnes The platforms to be excluded from the test
 */
gdjs.PlatformerObjectRuntimeBehavior.prototype._isCollidingWithExcluding = function(candidates, exceptTheseOnes)
{
    for (var i = 0;i<candidates.length;++i) {
        var platform = candidates[i];

        if (exceptTheseOnes && this._isIn(exceptTheseOnes, platform.owner.id)) continue;
        if ( platform.getPlatformType() === gdjs.PlatformRuntimeBehavior.LADDER ) continue;
        if ( gdjs.RuntimeObject.collisionTest(this.owner, platform.owner) )
            return true;
    }

    return false;
};

/**
 * Return (one of) the platform which is colliding with the behavior owner object.
 * Overlapped jump thru and ladders are excluded.
 * _updatePotentialCollidingObjects and _updateOverlappedJumpThru should have been called before.
 */
gdjs.PlatformerObjectRuntimeBehavior.prototype._getCollidingPlatform = function()
{
    for (var i = 0;i < this._potentialCollidingObjects.length;++i) {
        var platform = this._potentialCollidingObjects[i];

        if ( platform.getPlatformType() !== gdjs.PlatformRuntimeBehavior.LADDER
            && !this._isIn(this._overlappedJumpThru, platform.owner.id)
            && gdjs.RuntimeObject.collisionTest(this.owner, platform.owner) ) {
                return platform;
        }
    }

    return null; //Nothing is being colliding with the behavior object.
};

/**
 * Update _overlappedJumpThru member, so that it contains all the jumpthru platforms colliding with
 * the behavior owner object.
 * Note: _updatePotentialCollidingObjects must have been called before.
 * @private
 */
gdjs.PlatformerObjectRuntimeBehavior.prototype._updateOverlappedJumpThru = function()
{
    this._overlappedJumpThru.length = 0;
    for (var i = 0;i < this._potentialCollidingObjects.length;++i) {
        var platform = this._potentialCollidingObjects[i];

        if ( platform.getPlatformType() === gdjs.PlatformRuntimeBehavior.JUMPTHRU
            && gdjs.RuntimeObject.collisionTest(this.owner, platform.owner) ) {
                this._overlappedJumpThru.push(platform);
        }
    }
};

/**
 * Return true if the object is overlapping a ladder.
 * Note: _updatePotentialCollidingObjects must have been called before.
 * @private
 */
gdjs.PlatformerObjectRuntimeBehavior.prototype._isOverlappingLadder = function() {
    for (var i = 0;i < this._potentialCollidingObjects.length;++i) {
        var platform = this._potentialCollidingObjects[i];

        if ( platform.getPlatformType() !== gdjs.PlatformRuntimeBehavior.LADDER ) continue;
        if ( gdjs.RuntimeObject.collisionTest(this.owner, platform.owner) )
            return true;
    }

    return false;
};

gdjs.PlatformerObjectRuntimeBehavior.prototype._isIn = function(platformArray, id) {
    for (var i = 0;i < platformArray.length;++i) {
        if (platformArray[i].owner.id === id)
            return true;
    }

    return false;
}

/**
 * Update _potentialCollidingObjects member with platforms near the object.
 * @private
 */
gdjs.PlatformerObjectRuntimeBehavior.prototype._updatePotentialCollidingObjects = function(maxMovementLength)
{
    this._potentialCollidingObjects = this._manager.getAllPlatformsAround(this.owner, maxMovementLength);

    //This is the naive implementation when the platforms manager is simply containing a list
    //of all existing platforms:

    /*var o1w = this.owner.getWidth();
    var o1h = this.owner.getHeight();
    var obj1BoundingRadius = Math.sqrt(o1w*o1w+o1h*o1h)/2.0 + maxMovementLength;

    //Get all platforms and keep only
    var allPlatforms = this._manager.getAllPlatforms();
    for (var k in allPlatforms.items) {
        if (allPlatforms.items.hasOwnProperty(k)) {
            var obj2 = allPlatforms.items[k].owner;

            var o2w = obj2.getWidth();
            var o2h = obj2.getHeight();

            var x = this.owner.getDrawableX()+this.owner.getCenterX()-(obj2.getDrawableX()+obj2.getCenterX());
            var y = this.owner.getDrawableY()+this.owner.getCenterY()-(obj2.getDrawableY()+obj2.getCenterY());
            var obj2BoundingRadius = Math.sqrt(o2w*o2w+o2h*o2h)/2.0;

            if ( Math.sqrt(x*x+y*y) <= obj1BoundingRadius + obj2BoundingRadius ) {
                if ( !this._potentialCollidingObjects.hasOwnProperty(k) )
                    this._potentialCollidingObjects[k] = allPlatforms.items[k];
            }
            else {
                if ( this._potentialCollidingObjects.hasOwnProperty(k) )
                    delete this._potentialCollidingObjects[k];
            }
        }
    }*/
};

gdjs.PlatformerObjectRuntimeBehavior.prototype.simulateControl = function(input) {
    if ( input === "Left" ) this._leftKey = true;
    else if ( input === "Right" ) this._rightKey = true;
    else if ( input === "Up" ) this._upKey = true;
    else if ( input === "Down" ) this._downKey = true;
    else if ( input === "Ladder" ) this._ladderKey = true;
    else if ( input === "Jump" ) this._jumpKey = true;
};

gdjs.PlatformerObjectRuntimeBehavior.prototype.getGravity = function()
{
    return this._gravity;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.getMaxFallingSpeed = function()
{
    return this._maxFallingSpeed;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.getAcceleration = function()
{
    return this._acceleration;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.getDeceleration = function()
{
    return this._deceleration;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.getMaxSpeed = function()
{
    return this._maxSpeed;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.getJumpSpeed = function()
{
    return this._jumpSpeed;
};

gdjs.PlatformerObjectRuntimeBehavior.prototype.setGravity = function(gravity)
{
    this._gravity = gravity;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.setMaxFallingSpeed = function(maxFallingSpeed)
{
    this._maxFallingSpeed = maxFallingSpeed;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.setAcceleration = function(acceleration)
{
    this._acceleration = acceleration;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.setDeceleration = function(deceleration)
{
    this._deceleration = deceleration;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.setMaxSpeed = function(maxSpeed)
{
    this._maxSpeed = maxSpeed;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.setJumpSpeed = function(jumpSpeed)
{
    this._jumpSpeed = jumpSpeed;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.setSlopeMaxAngle = function(slopeMaxAngle)
{
    if (slopeMaxAngle < 0 || slopeMaxAngle >= 90) return;

    this._slopeMaxAngle = slopeMaxAngle;
    if ( slopeMaxAngle == 45 )
        this._slopeClimbingFactor = 1; //Avoid rounding errors
    else
        this._slopeClimbingFactor = Math.tan(slopeMaxAngle*3.1415926/180.0);
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.setCanJump = function()
{
    this._canJump = true;
};

gdjs.PlatformerObjectRuntimeBehavior.prototype.ignoreDefaultControls = function(ignore)
{
    this._ignoreDefaultControls = ignore;
};

gdjs.PlatformerObjectRuntimeBehavior.prototype.simulateLeftKey = function()
{
    this._leftKey = true;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.simulateRightKey = function()
{
    this._rightKey = true;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.simulateLadderKey = function()
{
    this._ladderKey = true;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.simulateUpKey = function()
{
    this._upKey = true;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.simulateDownKey = function()
{
    this._downKey = true;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.simulateJumpKey = function()
{
    this._jumpKey = true;
};

gdjs.PlatformerObjectRuntimeBehavior.prototype.isOnFloor = function()
{
    return this._isOnFloor;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.isOnLadder = function()
{
    return this._isOnLadder;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.isJumping = function()
{
    return this._jumping;
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.isFalling = function()
{
    return !this._isOnFloor && !this._isOnLadder && (!this._jumping || this._currentJumpSpeed < this._currentFallSpeed);
};
gdjs.PlatformerObjectRuntimeBehavior.prototype.isMoving = function()
{
    return (this._hasReallyMoved &&this._currentSpeed !== 0) || this._currentJumpSpeed !== 0 || this._currentFallSpeed !== 0;
};
