#include "Extensions.h"

//(*InternalHeaders(Extensions)







#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <wx/clntdata.h>

#include "GDL/ExtensionBase.h"
#include "GDL/Object.h"
#include "GDL/Game.h"
#include "GDL/ExtensionsManager.h"
#include "GDL/ExtensionsLoader.h"

#include "gdTreeItemStringData.h"

//(*IdInit(Extensions)


















BEGIN_EVENT_TABLE(Extensions,wxDialog)
	//(*EventTable(Extensions)
	//*)
END_EVENT_TABLE()

Extensions::Extensions(wxWindow* parent, Game & game_) :
game(game_)
{
	//(*Initialize(Extensions)




























































































	UpdateList();
}

Extensions::~Extensions()
{
	//(*Destroy(Extensions)
	//*)
}

void Extensions::UpdateList()
{
    ExtensionsList->Clear();

    gdp::ExtensionsManager * extensionsManager = gdp::ExtensionsManager::getInstance();
    const vector < boost::shared_ptr<ExtensionBase> > & extensionsInstalled = extensionsManager->GetExtensions();

    //Cr�er la liste � partir des extensions install�es
    for (unsigned int i = 0;i<extensionsInstalled.size();++i)
    {
        wxStringClientData * associatedData = new wxStringClientData(extensionsInstalled[i]->GetName());

        if ( extensionsInstalled[i]->GetName().find("Builtin") < extensionsInstalled[i]->GetName().length() )
            ExtensionsList->Insert(extensionsInstalled[i]->GetFullName(), ExtensionsList->GetCount(), associatedData);
        else
            ExtensionsList->Insert(extensionsInstalled[i]->GetFullName(), 0, associatedData);

    }

    for (unsigned int i =0;i<ExtensionsList->GetCount();++i)
    {
        wxStringClientData * associatedData = dynamic_cast<wxStringClientData*>(ExtensionsList->GetClientObject(i));
        if (associatedData)
        {
            if ( find(  game.extensionsUsed.begin(),
                        game.extensionsUsed.end(),
                        associatedData->GetData()) != game.extensionsUsed.end() )
            {
                ExtensionsList->Check(i, true);
            }
        }
    }
}


////////////////////////////////////////////////////////////
/// Installer une nouvelle extension
////////////////////////////////////////////////////////////
void Extensions::OninstallNewExtensionBtClick(wxCommandEvent& event)
{
}

////////////////////////////////////////////////////////////
/// D�sinstaller une extension :
/// Supprimer le fichier du r�pertoire d'extensions
////////////////////////////////////////////////////////////
void Extensions::OnuninstallExtensionBtClick(wxCommandEvent& event)
{
}

////////////////////////////////////////////////////////////
/// Clic sur une extension : Affichage des informations
////////////////////////////////////////////////////////////
void Extensions::OnExtensionsListSelect(wxCommandEvent& event)
{
    int id = event.GetSelection();
    wxStringClientData * associatedData = dynamic_cast<wxStringClientData*>(ExtensionsList->GetClientObject(id));

    if (associatedData == NULL) return;

    gdp::ExtensionsManager * extensionsManager = gdp::ExtensionsManager::getInstance();
    const vector < boost::shared_ptr<ExtensionBase> > & extensionsInstalled = extensionsManager->GetExtensions();

    for (unsigned int i = 0;i<extensionsInstalled.size();++i)
    {
        if ( extensionsInstalled[i]->GetName() == associatedData->GetData() )
        {
            infoEdit->SetValue(extensionsInstalled[i]->GetInfo());
            authorTxt->SetLabel(extensionsInstalled[i]->GetAuthor());
            licenseTxt->SetLabel(extensionsInstalled[i]->GetLicense());
            return;
        }
    }
}

////////////////////////////////////////////////////////////
/// Fermeture : Chargement des extensions et ajout � la liste
/// des extensions du jeu
////////////////////////////////////////////////////////////
void Extensions::OnFermerBtClick(wxCommandEvent& event)
{
    game.extensionsUsed.clear();

    for (unsigned int i =0;i<ExtensionsList->GetCount();++i)
    {
    	if ( ExtensionsList->IsChecked(i) )
        {
            wxStringClientData * associatedData = dynamic_cast<wxStringClientData*>(ExtensionsList->GetClientObject(i));

            if (associatedData)
                game.extensionsUsed.push_back(string(associatedData->GetData().mb_str()));
        }
    }

    EndModal(0);
}