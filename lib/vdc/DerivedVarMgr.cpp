#include <cassert>
#include <sstream>
#include "vapor/DerivedVarMgr.h"

using namespace VAPoR;

DerivedVarMgr::DerivedVarMgr() {
	_ovrName = "";
}

int DerivedVarMgr::initialize(
	const std::vector <string> &,
	const std::vector <string> &
) {
	return(0);
}

void DerivedVarMgr::AddCoordVar(DerivedCoordVar *cvar) {
	vector <string> names = cvar->GetNames();

	for (int i=0; i<names.size(); i++) {
		_coordVars[names[i]] = cvar;
		_vars[names[i]] = cvar;
	}
}
    
void DerivedVarMgr::AddDataVar(DerivedDataVar *dvar) {
	vector <string> names = dvar->GetNames();

	for (int i=0; i<names.size(); i++) {
		_dataVars[names[i]] = dvar;
		_vars[names[i]] = dvar;
	}
}

void DerivedVarMgr::RemoveVar(const DerivedVar *var) {

	bool done = false;
	while (! done) {
		map<string, DerivedVar *>::iterator itr;
		done = true;
		for (itr = _vars.begin(); itr!=_vars.end(); ++itr) {
			if (itr->second == var) {
				_vars.erase(itr);
				done = false;
				break;
			}
		}
	}

	done = false;
	while (! done) {
		map<string, DerivedCoordVar *>::iterator itr;
		done = true;
		for (itr = _coordVars.begin(); itr!=_coordVars.end(); ++itr) {
			if (itr->second == var) {
				_coordVars.erase(itr);
				done = false;
				break;
			}
		}
	}

	done = false;
	while (! done) {
		map<string, DerivedDataVar *>::iterator itr;
		done = true;
		for (itr = _dataVars.begin(); itr!=_dataVars.end(); ++itr) {
			if (itr->second == var) {
				_dataVars.erase(itr);
				done = false;
				break;
			}
		}
	}

}

bool DerivedVarMgr::getCoordVarInfo(
	string varname, DC::CoordVar &cvarInfo
) const {

	DerivedCoordVar *dvar = _getCoordVar(varname);
	if (! dvar) return(false);

	return(dvar->GetCoordVarInfo(varname, cvarInfo));
}

bool DerivedVarMgr::getDataVarInfo(
	string varname, DC::DataVar &dvarInfo
) const {

	DerivedDataVar *dvar = _getDataVar(varname);
	if (! dvar) return(false);

	return(dvar->GetDataVarInfo(varname, dvarInfo));
}

bool DerivedVarMgr::getBaseVarInfo(string varname, DC::BaseVar &varInfo) const {

	DerivedVar *var = _getVar(varname);
	if (! var) return(false);

	return(var->GetBaseVarInfo(varname, varInfo));
}

std::vector <string> DerivedVarMgr::getDataVarNames() const {;
	map<string,  DerivedDataVar *>::const_iterator itr;

	vector <string> names;
	for (itr=_dataVars.begin(); itr!=_dataVars.end(); ++itr) {
		names.push_back(itr->first);
	}
	return(names);
}

std::vector <string> DerivedVarMgr::getCoordVarNames() const {;
	map<string,  DerivedCoordVar *>::const_iterator itr;

	vector <string> names;
	for (itr=_coordVars.begin(); itr!=_coordVars.end(); ++itr) {
		names.push_back(itr->first);
	}
	return(names);
}

size_t DerivedVarMgr::getNumRefLevels(string varname) const {
	DerivedVar *var = _getVar(varname);
	if (! var) return(0);

	return(var->GetNumRefLevels(varname));

}

bool DerivedVarMgr::getAtt(
	string varname, string attname, vector <double> &values
) const {
	DerivedVar *var = _getVar(varname);
	if (! var) return(false);

	return(var->GetAtt(varname, attname, values));
}

bool DerivedVarMgr::getAtt(
	string varname, string attname, vector <long> &values
) const {
	DerivedVar *var = _getVar(varname);
	if (! var) return(false);

	return(var->GetAtt(varname, attname, values));
}

bool DerivedVarMgr::getAtt(
	string varname, string attname, string &values
) const {
	DerivedVar *var = _getVar(varname);
	if (! var) return(false);

	return(var->GetAtt(varname, attname, values));
}

std::vector <string> DerivedVarMgr::getAttNames(string varname) const {

	DerivedVar *var = _getVar(varname);
	if (! var) return(vector <string> ());

	return(var->GetAttNames(varname));
}

DC::XType DerivedVarMgr::getAttType(string varname, string attname) const {

	DerivedVar *var = _getVar(varname);
	if (! var) return(DC::INVALID);

	return(var->GetAttType(varname, attname));
}

int DerivedVarMgr::getDimLensAtLevel(
	string varname, int level, std::vector <size_t> &dims_at_level,
	std::vector <size_t> &bs_at_level
) const {

	DerivedVar *var = _getVar(varname);
	if (! var) return(-1);

	return(var->GetDimLensAtLevel(varname, level, dims_at_level, bs_at_level));
}

int DerivedVarMgr::openVariableRead(
	size_t ts, string varname, int level, int lod
) {
	_ovrName = varname;
	_ovrLevel = level;
	DerivedVar *var = _getVar(varname);
	if (! var) {
		SetErrMsg("Invalid variable : %s", varname.c_str());
		return(-1);
	}

	return(var->OpenVariableRead(ts, varname, level, lod));
}

int DerivedVarMgr::closeVariable(int fd) {
	DerivedVar *var = _getVar(_ovrName);

	if (! var) {
		SetErrMsg("Invalid variable : %s", _ovrName.c_str());
		return(-1);
	}
	_ovrName.clear();

	return(var->CloseVariable(fd));
}

int DerivedVarMgr::readRegion(
	int fd,
	const vector <size_t> &min, const vector <size_t> &max, float *region
) {
	DerivedVar *var = _getVar(_ovrName);
	if (! var) {
		SetErrMsg("Invalid variable : %s", _ovrName.c_str());
		return(-1);
	}

	return(var->ReadRegion(fd, min, max, region));
}

int DerivedVarMgr::readRegionBlock(
	int fd,
	const vector <size_t> &min, const vector <size_t> &max, float *region
) {
	DerivedVar *var = _getVar(_ovrName);
	if (! var) {
		SetErrMsg("Invalid variable : %s", _ovrName.c_str());
		return(-1);
	}

	return(var->ReadRegionBlock(fd, min, max, region));
}

bool DerivedVarMgr::variableExists(
	size_t ts,
	string varname,
	int reflevel,
	int lod
) const {
	DerivedVar *var = _getVar(varname);
	if (! var) return(false);

	return(var->VariableExists(ts, varname, reflevel, lod));
}

DerivedVar *DerivedVarMgr::_getVar(string name) const {
	map<string,  DerivedVar *>::const_iterator itr;

	itr = _vars.find(name);
	if (itr == _vars.end()) return(NULL);

	return(itr->second);
}

DerivedDataVar *DerivedVarMgr::_getDataVar(string name) const {
	map<string,  DerivedDataVar *>::const_iterator itr;

	itr = _dataVars.find(name);
	if (itr == _dataVars.end()) return(NULL);

	return(itr->second);
}


DerivedCoordVar *DerivedVarMgr::_getCoordVar(string name) const {
	map<string,  DerivedCoordVar *>::const_iterator itr;

	itr = _coordVars.find(name);
	if (itr == _coordVars.end()) return(NULL);

	return(itr->second);
}
