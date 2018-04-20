#pragma once



bool	hookFunction( void* targetFunction , void* pNewFcuntionAddress );
bool	unhookFunction( void* targetFunction );

