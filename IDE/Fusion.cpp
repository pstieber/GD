#include "Fusion.h"

#ifdef DEBUG
#include "nommgr.h"
#endif

//(*InternalHeaders(Fusion)






#include <wx/filedlg.h>
#include <string>
#include <vector>
#include <wx/log.h>
#include <wx/msgdlg.h>

#ifdef DEBUG

#endif

#include "StdAlgo.h"
#include "GDL/Game.h"
#include "GDL/ChercherScene.h"
#include "GDL/OpenSaveGame.h"


using namespace std;

//(*IdInit(Fusion)












BEGIN_EVENT_TABLE(Fusion,wxDialog)
	//(*EventTable(Fusion)
	//*)
END_EVENT_TABLE()

Fusion::Fusion(wxWindow* parent, Game * pJeu)
{
	//(*Initialize(Fusion)























































	jeu = pJeu;
}

Fusion::~Fusion()
{
	//(*Destroy(Fusion)
	//*)
}


void Fusion::OnFermerBtClick(wxCommandEvent& event)
{
    EndModal(0);
}

void Fusion::OnFusionBtClick(wxCommandEvent& event)
{
    wxFileDialog dialog( this, _( "Choisissez le jeu � ouvrir" ), "", "", "*.jgd" );
    dialog.ShowModal();

    if ( dialog.GetPath() == "" )
        return;

    Game secondGame;
    OpenSaveGame open(secondGame);
    open.OpenFromFile(static_cast<string>( dialog.GetPath() ));

    if ( ImageCheck->GetValue() )
    {
        for(unsigned int i = 0;i<secondGame.images.size();i++)
        {
            if ( ChercherNomImage( jeu->images, secondGame.images.at(i).nom ) != -1)
            {
                wxString depart = _("Une image nomm� \"");
                wxString fin = _("\" est d�j� pr�sente dans le jeu. Voulez vous la remplacer ?");
                if (wxMessageBox(depart+secondGame.images.at(i).nom+fin, "Une image de ce nom existe d�j�",wxYES_NO ) == wxYES)
                {
                    //Remplacement
                    jeu->images.erase(jeu->images.begin() + ChercherNomImage( jeu->images, secondGame.images.at(i).nom ));
                    jeu->images.push_back(secondGame.images.at(i));
                }

            }
            else
                jeu->images.push_back(secondGame.images.at(i));
        }
    }
    if ( ScenesCheck->GetValue() )
    {
        for(unsigned int i = 0;i<secondGame.m_scenes.size();i++)
        {
            if ( ChercherScene( jeu->m_scenes, secondGame.m_scenes.at(i).name) != -1)
            {
                wxString depart = _("Une sc�ne nomm� \"");
                wxString fin = _("\" est d�j� pr�sente dans le jeu. Voulez vous la remplacer ?");
                if (wxMessageBox(depart+secondGame.m_scenes.at(i).name+fin, "Une sc�ne de ce nom existe d�j�",wxYES_NO ) == wxYES)
                {
                    //Remplacement
                    jeu->m_scenes.erase(jeu->m_scenes.begin() + ChercherScene( jeu->m_scenes, secondGame.m_scenes.at(i).name));
                    jeu->m_scenes.push_back(secondGame.m_scenes.at(i));
                }
            }
            else
                jeu->m_scenes.push_back(secondGame.m_scenes.at(i));
        }
    }

    wxLogMessage(_("Le jeu a �t� correctement fusionn�."));
}