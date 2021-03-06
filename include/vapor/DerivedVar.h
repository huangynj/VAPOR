#include <iostream>
#include <vapor/DC.h>
#include <vapor/MyBase.h>
#include <vapor/Proj4API.h>

#ifndef	_DERIVEDVAR_H_
#define	_DERIVEDVAR_H_

namespace VAPoR {

class NetCDFCollection;

//!
//! \class DerivedVar
//!
//! \brief Derived variable abstract class
//!
//! \author John Clyne
//! \date    January, 2017
//!
//!
class VDF_API DerivedVar : public Wasp::MyBase {
public:

 DerivedVar(string varName) {
	_derivedVarNames.push_back(varName);
 };

 DerivedVar(const std::vector <string> &varNames) {
	_derivedVarNames = varNames;
 };

 virtual ~DerivedVar() {}

 virtual int Initialize() = 0;

 std::vector <string> GetNames() const {
	return(_derivedVarNames);
 }

 virtual bool GetBaseVarInfo(string varname, DC::BaseVar &var) const = 0;

 virtual size_t GetNumRefLevels(string varname) const {
	return(1);
 }

 virtual bool GetAtt(
	string varname, string attname, std::vector <double> &values
 ) const {
	values.clear();
	return(false);
 }

 virtual bool GetAtt(
	string varname, string attname, std::vector <long> &values
 ) const {
	values.clear();
	return(false);
 }

 virtual bool GetAtt(
	string varname, string attname, string &values
 ) const {
	values.clear();
	return(false);
 }

 virtual std::vector <string> GetAttNames(string varname) const {
	return(std::vector <string> ());
 }

 virtual DC::XType GetAttType(string varname, string attname) const {
	return(DC::INVALID);
 }

 virtual std::vector <string> GetInputs() const = 0;

 virtual int GetDimLensAtLevel(
	string varname, int level, std::vector <size_t> &dims_at_level,
	std::vector <size_t> &bs_at_level
 ) const = 0;

 virtual int OpenVariableRead(
	size_t ts, string varname, int level=0, int lod=0
 ) = 0;

 virtual int CloseVariable(int fd) = 0;
 
 virtual int ReadRegionBlock(
	int fd,
	const std::vector <size_t> &min, const std::vector <size_t> &max,
	float *region
 ) = 0;


 virtual int ReadRegion(
	int fd,
	const std::vector <size_t> &min, const std::vector <size_t> &max,
	float *region
 ) = 0;

 virtual bool VariableExists(
	size_t ts,
	string varname,
	int reflevel,
	int lod
 ) const = 0;

protected:
 std::vector <string> _derivedVarNames;
 DC::FileTable _fileTable;

};

//!
//! \class DerivedCoordVar
//!
//! \brief Derived coordinate variable abstract class
//!
//! \author John Clyne
//! \date   Februrary, 2018 
//!
//!
class VDF_API DerivedCoordVar : public DerivedVar {
public:

 DerivedCoordVar(const std::vector <string> &varNames) : DerivedVar(varNames) {}
 DerivedCoordVar(string varName) : DerivedVar(varName) {}
 virtual ~DerivedCoordVar() {}

 virtual bool GetCoordVarInfo(string varname, DC::CoordVar &cvar) const = 0;

};

//!
//! \class DerivedDataVar
//!
//! \brief Derived data variable abstract class
//!
//! \author John Clyne
//! \date   Februrary, 2018 
//!
//!
class VDF_API DerivedDataVar : public DerivedVar {
public:

 DerivedDataVar(const std::vector <string> &varNames) : DerivedVar(varNames) {}
 DerivedDataVar(string varName) : DerivedVar(varName) {}
 virtual ~DerivedDataVar() {}

 virtual bool GetDataVarInfo(string varname, DC::DataVar &cvar) const = 0;

};

//!
//! \class DerivedCoordVar_PCSFromLatLon
//!
//! \brief Derived PCS coordinate variable from lat-lon coordinate pairs
//!
//! \author John Clyne
//! \date   Februrary, 2018 
//!
//!
class VDF_API DerivedCoordVar_PCSFromLatLon : public DerivedCoordVar {
public:
 DerivedCoordVar_PCSFromLatLon(
	const std::vector <string> &derivedVarNames, DC *dc,
	std::vector <string> inNames, string proj4String, bool uGridFlag
 );
 virtual ~DerivedCoordVar_PCSFromLatLon() {}

 virtual int Initialize();

 virtual bool GetBaseVarInfo(string varname, DC::BaseVar &var) const;

 virtual bool GetCoordVarInfo(string varname, DC::CoordVar &cvar) const;

 virtual std::vector <string> GetInputs() const {
	return(std::vector <string> {_lonName, _latName});
 }

 virtual int GetDimLensAtLevel(
	string varname, int level, std::vector <size_t> &dims_at_level,
	std::vector <size_t> &bs_at_level
 ) const;

 virtual int OpenVariableRead(
	size_t ts, string varname, int level=0, int lod=0
 );

 virtual int CloseVariable(int fd);
 
 virtual int ReadRegionBlock(
	int fd,
	const std::vector <size_t> &min, const std::vector <size_t> &max,
	float *region
 );

 virtual int ReadRegion(
	int fd,
	const std::vector <size_t> &min, const std::vector <size_t> &max,
	float *region
 ) {
	return(ReadRegionBlock(fd, min, max, region));
 }

 virtual bool VariableExists(
	size_t ts,
	string varname,
	int reflevel,
	int lod
 ) const;

private:
 DC *_dc;
 string	_proj4String;
 string _lonName;
 string _latName;
 string _xCoordName;
 string _yCoordName;
 bool _make2DFlag;
 bool _uGridFlag;
 std::vector <size_t> _dimLens;
 std::vector <size_t> _bs;
 Proj4API	_proj4API;
 DC::CoordVar	_xCoordVarInfo;
 DC::CoordVar	_yCoordVarInfo;

 int _setupVar();
 int _getVarBlock(
	size_t ts, string varname, int level, int lod,
	const std::vector <size_t> &min, const std::vector <size_t> &max,
	float *region
 );

 int _readRegionBlockHelper1D(
	DC::FileTable::FileObject *f,
	const std::vector <size_t> &min, const std::vector <size_t> &max,
	float *region
 );
 int _readRegionBlockHelper2D(
	DC::FileTable::FileObject *f,
	const std::vector <size_t> &min, const std::vector <size_t> &max,
	float *region
 );
 
};

//!
//! \class DerivedCoordVar_CF1D
//!
//! \brief Derived 1D CF conventions coordinate variable using 
//! grid coordinates
//!
//! \author John Clyne
//! \date   Februrary, 2018 
//!
//!
class VDF_API DerivedCoordVar_CF1D : public DerivedCoordVar {
public:
 DerivedCoordVar_CF1D(
	const std::vector <string> &derivedVarNames, DC *dc,
	string dimName, int axis, string units
 );
 virtual ~DerivedCoordVar_CF1D() {}

 virtual int Initialize();

 virtual bool GetBaseVarInfo(string varname, DC::BaseVar &var) const;

 virtual bool GetCoordVarInfo(string varname, DC::CoordVar &cvar) const;

 virtual std::vector <string> GetInputs() const {
	return(std::vector <string> ());
 }

 virtual int GetDimLensAtLevel(
	string varname, int level, std::vector <size_t> &dims_at_level,
	std::vector <size_t> &bs_at_level
 ) const;

 virtual int OpenVariableRead(
	size_t ts, string varname, int level=0, int lod=0
 );

 virtual int CloseVariable(int fd);
 
 virtual int ReadRegionBlock(
	int fd,
	const std::vector <size_t> &min, const std::vector <size_t> &max,
	float *region
 );

 virtual int ReadRegion(
	int fd,
	const std::vector <size_t> &min, const std::vector <size_t> &max,
	float *region
 ) {
	return(ReadRegionBlock(fd, min, max, region));
 }

 virtual bool VariableExists(
	size_t ts,
	string varname,
	int reflevel,
	int lod
 ) const;

private:
 DC *_dc;
 string _dimName;
 string _coordName;
 size_t _dimLen;
 DC::CoordVar	_coordVarInfo;

};

//!
//! \class DerivedCoordVar_WRFTime
//!
//! \brief Derived WRF Time coordinate variable 
//!
//! \author John Clyne
//! \date   Februrary, 2018 
//!
//!
class VDF_API DerivedCoordVar_WRFTime : public DerivedCoordVar {
public:
 DerivedCoordVar_WRFTime(
	string derivedVarName, NetCDFCollection *ncdfc,
	string wrfTimeVar, string dimName, float p2si = 1.0
 );
 virtual ~DerivedCoordVar_WRFTime() {}

 virtual int Initialize();

 virtual bool GetBaseVarInfo(string varname, DC::BaseVar &var) const;

 virtual bool GetCoordVarInfo(string varname, DC::CoordVar &cvar) const;

 virtual std::vector <string> GetInputs() const {
	return(std::vector <string> ());
 }

 virtual int GetDimLensAtLevel(
	string varname, int level, std::vector <size_t> &dims_at_level,
	std::vector <size_t> &bs_at_level
 ) const;

 virtual int OpenVariableRead(
	size_t ts, string varname, int level=0, int lod=0
 );

 virtual int CloseVariable(int fd);
 
 virtual int ReadRegionBlock(
	int fd,
	const std::vector <size_t> &min, const std::vector <size_t> &max,
	float *region
 );

 virtual int ReadRegion(
	int fd,
	const std::vector <size_t> &min, const std::vector <size_t> &max,
	float *region
 ) {
	return(ReadRegionBlock(fd, min, max, region));
 }

 virtual bool VariableExists(
	size_t ts,
	string varname,
	int reflevel,
	int lod
 ) const;

 size_t TimeLookup(size_t ts) const {
	return(ts < _timePerm.size() ? _timePerm[ts] : 0);
 }

private:
 NetCDFCollection *_ncdfc;
 std::vector <float> _times;
 std::vector <int> _timePerm;
 string _coordName;
 string _wrfTimeVar;
 float _p2si;
 size_t _ovr_ts;
 DC::CoordVar	_coordVarInfo;
 
};


class VDF_API DerivedCoordVar_Staggered : public DerivedCoordVar {
public:
 DerivedCoordVar_Staggered(
	string derivedVarName, string stagDimName,
	DC *dc, string inName, string dimName
 );
 virtual ~DerivedCoordVar_Staggered() {}

 virtual int Initialize();

 virtual bool GetBaseVarInfo(string varname, DC::BaseVar &var) const;

 virtual bool GetCoordVarInfo(string varname, DC::CoordVar &cvar) const;

 virtual std::vector <string> GetInputs() const {
	return(std::vector <string> ());
 }

 virtual int GetDimLensAtLevel(
	string varname, int level, std::vector <size_t> &dims_at_level,
	std::vector <size_t> &bs_at_level
 ) const;

 virtual int OpenVariableRead(
	size_t ts, string varname, int level=0, int lod=0
 );

 virtual int CloseVariable(int fd);
 
 virtual int ReadRegionBlock(
	int fd,
	const std::vector <size_t> &min, const std::vector <size_t> &max,
	float *region
 ) {
	return(ReadRegion(fd, min, max, region));
 }

 virtual int ReadRegion(
	int fd,
	const std::vector <size_t> &min, const std::vector <size_t> &max,
	float *region
 );

 virtual bool VariableExists(
	size_t ts,
	string varname,
	int reflevel,
	int lod
 ) const;

private:
 string _derivedVarName;
 string _inName;
 string _stagDimName;
 string _dimName;
 DC *_dc;
 DC::CoordVar _coordVarInfo;
 int _stagDim;

 void _transpose(
	const float *a, float *b, std::vector <size_t> inDims,
	int axis
 ) const;
 void _transpose(
	std::vector <size_t> inDims, int axis, std::vector <size_t> &outDims
 ) const;
 
};

};

#endif
