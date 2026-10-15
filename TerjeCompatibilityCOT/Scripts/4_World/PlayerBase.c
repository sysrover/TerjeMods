modded class PlayerBase
{
	override void COTRemoveAllDiseases()
	{
		super.COTRemoveAllDiseases();

		// Shared by player-manager healing, ESP healing and Remove All Diseases.
		if (g_Game && g_Game.IsServer())
			GetTerjeAdmintoolSupport().OnHeal(this);
	}
}
