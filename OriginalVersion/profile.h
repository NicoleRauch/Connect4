/*************************************************************************\
 * profile.h
 *
 * (c) 1994 by Ralf Seidel
 * Set tabs to 3 to get a readable source.
\*************************************************************************/
#ifdef INITIALIZE
#undef PUBLIC
#define PUBLIC
#endif

PUBLIC BOOL ReadProfile( VOID );

PUBLIC BOOL WriteProfile( VOID );

PUBLIC BOOL StoreWindowPos( const HINI hini, const PSZ pszAppName,
									 const PCSZ pszKeyName, const HWND hwnd );

PUBLIC BOOL RestoreWindowPos( const HINI hini, const PSZ pszAppName,
										const PCSZ pszKeyName, const HWND hwnd );

PUBLIC BOOL SaveGame();

PUBLIC BOOL LoadGame();

#if 0
PUBLIC inline BOOL DeleteAppFromProfile( const HINI hini )
/*************************************************************************\
 * function DeleteAppFromProfile
 * Abstract:
 * This function is used to delete the application entry in the profile
 * specified by hini. hini is a handle returned by PrfOpenProfile or one
 * of the following HINI_USERINI or HINI_SYSTEMPROFILE. Especially for
 * those two ini files it is good behavior to delete entrys you don't
 * need anymore.
 * Note: This function isn't any longer included since I don't write
 * to other profiles then to the ini file named connect4.ini.
\*************************************************************************/
{
	return PrfWriteProfileString( hini, pszAppName, NULL, NULL );
}
#endif

