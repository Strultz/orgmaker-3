#include "Update.h"
#include <windows.h>
#include <wininet.h>
#include <sstream>
#include <string>

#include "vendor/json.hpp"

#include "DefOrg.h"
#include "Setting.h"

using json = nlohmann::json;

static char tVerName[64];
static bool canUpdateCheck = true;

struct Version {
	int major;
	int minor;
	int patch;
	int pre;
};

static Version GetVersion(const std::string& s, bool pre) {
	std::stringstream ss(s);
	Version v = {};
	char d;

	ss >> v.major >> d
		>> v.minor >> d
		>> v.patch;

	if (pre) {
		std::string preText;
		ss >> d >> preText;

		if (preText.rfind("pre", 0) != 0) {
			v.major = -1;
			return v;
		}

		v.pre = std::stoi(preText.substr(3));
	}
	else {
		v.pre = -1;
	}

	if (ss.fail()) {
		v.major = -1;
		return v;
	}

	return v;
}

static bool IsNewerVersion(const Version& current, const Version& latest) {
	if (latest.major > current.major) return true;
	if (latest.major < current.major) return false;

	if (latest.minor > current.minor) return true;
	if (latest.minor < current.minor) return false;

	if (latest.patch > current.patch) return true;
	if (latest.patch < current.patch) return false;

	if (current.pre != latest.pre) {
		if (current.pre == -1) return false;
		if (latest.pre == -1) return true;
		if (latest.pre > current.pre) return true;
		if (latest.pre < current.pre) return false;
	}

	return false;
}

void CheckUpdate(bool act) {
#ifdef _DEBUG
	if (!canUpdateCheck) return;
	canUpdateCheck = false;
	if (act) PostMessage(hWnd, OWM_UPDATESTATUS, 4, 0);
	canUpdateCheck = true;
	return;
#else
	if (!canUpdateCheck) return;
	canUpdateCheck = false;

	HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
	BOOL bResult = FALSE;
	DWORD dwBytesRead;
	char buffer[1024];

	hSession = InternetOpen("OrgMaker/3.x", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hSession == NULL) {
		canUpdateCheck = true;
		return;
	}

	hConnect = InternetConnect(hSession, "api.github.com", INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (hConnect == NULL) {
		InternetCloseHandle(hSession);
		// these status codes are magic numbers and should not be but i'm too lazy to fix it
		if (act) PostMessage(hWnd, OWM_UPDATESTATUS, 1, 0); //MessageBox(hWnd, "Request failed. Please try again later.", "OrgMaker Update", MB_ICONERROR | MB_OK);
		canUpdateCheck = true;
		return;
	}

	hRequest = HttpOpenRequest(hConnect, "GET", "/repos/Strultz/orgmaker-3/releases", NULL, NULL, NULL, 0, 0);
	if (hRequest == NULL) {
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hSession);
		if (act) PostMessage(hWnd, OWM_UPDATESTATUS, 1, 0); //MessageBox(hWnd, "Request failed. Please try again later.", "OrgMaker Update", MB_ICONERROR | MB_OK);
		canUpdateCheck = true;
		return;
	}

	bResult = HttpSendRequest(hRequest, NULL, 0, NULL, 0);
	if (!bResult) {
		InternetCloseHandle(hRequest);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hSession);
		if (act) PostMessage(hWnd, OWM_UPDATESTATUS, 1, 0); //MessageBox(hWnd, "Request failed. Please try again later.", "OrgMaker Update", MB_ICONERROR | MB_OK);
		canUpdateCheck = true;
		return;
	}

	std::string content;
	while ((bResult = InternetReadFile(hRequest, buffer, sizeof(buffer), &dwBytesRead)) && dwBytesRead > 0) {
		content.append(buffer, dwBytesRead);
	}

	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hSession);

	// ok fine I added the json library
	bool allowPrerelease = VER_IS_PRERELEASE;

	try {
		json j = json::parse(content);
		if (!j.is_array() || j.empty()) {
			if (act) PostMessage(hWnd, OWM_UPDATESTATUS, 2, 0);
			canUpdateCheck = true;
			return;
		}
		
		for (const auto& release : j) {
			bool isPrerelease = release.value("prerelease", false);

			if (allowPrerelease || !isPrerelease) {
				const std::string& tag = release.value("tag_name", "");

				Version current = { VER_MAJOR, VER_MINOR, VER_PATCH, VER_PRE };
				Version latest = GetVersion(tag, isPrerelease);

				if (latest.major == -1) {
					if (act) PostMessage(hWnd, OWM_UPDATESTATUS, 2, 0); //MessageBox(hWnd, "Request failed. Please try again later.", "OrgMaker Update", MB_ICONERROR | MB_OK);
					canUpdateCheck = true;
					return;
				}

				if (IsNewerVersion(current, latest)) {
					strncpy(tVerName, tag.c_str(), 63);
					tVerName[63] = '\0';

					PostMessage(hWnd, OWM_UPDATESTATUS, 0, (LPARAM)tVerName);
				}
				else {
					if (act) PostMessage(hWnd, OWM_UPDATESTATUS, 3, 0);
					canUpdateCheck = true;
					return;
				}

				canUpdateCheck = true;
				return;
			}
			else {
				if (act) PostMessage(hWnd, OWM_UPDATESTATUS, 3, 0);
				canUpdateCheck = true;
				return;
			}
		}

		if (act) PostMessage(hWnd, OWM_UPDATESTATUS, 2, 0);
		canUpdateCheck = true;
		return;
	}
	catch (...) {
		if (act) PostMessage(hWnd, OWM_UPDATESTATUS, 1, 0); //MessageBox(hWnd, "Request failed. Please try again later.", "OrgMaker Update", MB_ICONERROR | MB_OK);
		canUpdateCheck = true;
		return;
	}

	// Now we have the tag name
	canUpdateCheck = true;
	return;
#endif
}
