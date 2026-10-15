modded class JMPlayerFormTabGeneral
{
	override void ApplyStats(TStringArray guids)
	{
		super.ApplyStats(guids);

		if (m_Form)
			m_Form.TerjeApplyStats(guids);
	}
}
