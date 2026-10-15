modded class JMESPActionMenu
{
	protected override void BuildMain()
	{
		super.BuildMain();

		if (!m_Meta || !m_Menu)
			return;

		EntityAI uam_TargetEntity = EntityAI.Cast(m_Meta.target);
		if (!uam_TargetEntity || !GetTerjeScriptableAreas())
			return;

		float uam_Radiation = GetTerjeScriptableAreas().GetTerjeRadiationFromEntity(uam_TargetEntity);
		m_Menu.AddItem("terje:radiation", "Radiation: " + FloatToString(uam_Radiation));
		m_Menu.SetItemEnabled("terje:radiation", false);
	}
}
