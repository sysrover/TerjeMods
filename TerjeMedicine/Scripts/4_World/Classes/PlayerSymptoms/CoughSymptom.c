modded class CoughSymptom
{
	override bool IsContaminationActive()
	{
		if (m_Player.GetTerjeStats().GetBiohazardValue() >= 2)
			return true;
		return false;
	}
}
