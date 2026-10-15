modded class JMPlayerModule
{
	void SetTerjeValue( string id, float value, array< string > guids )
	{
		GetTerjeRPC().SendToServer("JM_COT_SetTerjeValue", new Param3<string, float, array<string>>(id, value, guids));
	}
}