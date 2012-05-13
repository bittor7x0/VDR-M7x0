/*
 *  $Id: parsing.c,v 1.3 2005/01/30 12:52:52 beejay Exp $
 */

#include "parsing.h"

#define ATTRIB_OPT_STRING(_attr,_target) \
        if (attrs.find(_attr) != attrs.end()) { \
                _target = attrs[_attr]; \
        }
#define ATTRIB_MAN_STRING(_attr,_target) \
        ATTRIB_OPT_STRING(_attr,_target) \
        else { \
                esyslog("ERROR : weatherng: Mandatory attribute %s missing in tag %s", \
                                _attr, name.c_str()); \
                return false; \
        }

static std::vector<std::string> context;
bool locked, IsDay, IsWind;
int dpart=0;
int count=0;
std::string param[255][255][2];
std::string ipart,day,date,town,unit1,unit2;

bool xStartElem(const std::string &name, std::map<std::string, std::string> &attrs) {
	if (name=="part") {
		ATTRIB_MAN_STRING("p",ipart);
		if (ipart=="d")
		{
			dpart=0;
		}
		else {
			dpart=1;
		}
	}
	if ( name=="us" || name=="ut" || name=="sunr" || name=="suns" || name=="icon" || name=="dnam" || name=="hi" || name=="low" || name=="ppcp" || name=="hmid" || name=="t" || name=="s") {
		locked=false;
	}

	if (name=="wind") {
		locked=false;
		IsWind=true;
	}
	if(name=="day") {
		if (++count < 255) {
			ATTRIB_MAN_STRING("t",param[count][6][0]);
			param[count][6][1]=param[count][6][0].c_str();
			ATTRIB_MAN_STRING("dt",param[count][7][0]);
			param[count][7][1]=param[count][7][0].c_str();
			IsDay=true;

		}
	}
	context.push_back(name);
	return true;
}

bool xCharData(const std::string &text) {
	if(context.size()>0 ) {
		if (context[context.size()-1]=="t" && IsWind==false && IsDay==true) {
			param[count][4][dpart]=text.c_str();
		}

		if (context[context.size()-1]=="t" && IsWind==true && IsDay==true) {
			param[count][8][dpart]=text.c_str();
		}

		if (context[context.size()-1]=="s" && IsWind==true && IsDay==true) {
			param[count][9][dpart]=text.c_str();
		}
		if (context[context.size()-1]=="hi" && IsDay==true) {
			dsyslog("DEBUG : weatherng: hi Wert: %s\n",text.c_str());
			param[count][2][0]=text.c_str();
			param[count][2][1]=text.c_str();

		}
		if (context[context.size()-1]=="low" && IsDay==true) {
			param[count][3][0]=text.c_str();
			param[count][3][1]=text.c_str();
		}
		if (context[context.size() -1]=="hmid" && IsDay==true) {
			param[count][10][dpart]=text.c_str();
		}
		if (context[context.size() -1]=="ppcp" && IsDay==true) {
			param[count][11][dpart]=text.c_str();
		}

	} // if(context.size()>0 ) {
	if (locked==false) {
		if(context.size()>0 ) {
			if (context[context.size()-1]=="dnam") {
				dsyslog("DEBUG : weatherng: dnam: %s\n",text.c_str());
				town=text.c_str();
			}

			if (context[context.size()-1]=="ut") {
				dsyslog("DEBUG : weatherng: unit 1: %s\n",text.c_str());
				unit1=text.c_str();
			}
			if (context[context.size()-1]=="us") {
				dsyslog("DEBUG : weatherng: unit 2: %s\n",text.c_str());
				unit2=text.c_str();
			}

			if (context[context.size()-1]=="sunr") {
				param[count][0][0]= text.c_str();
				param[count][0][1]= text.c_str();
			}
			if (context[context.size() -1]=="suns") {
				param[count][1][0]=text.c_str();
				param[count][1][1]=text.c_str();
			}
			if (context[context.size()-1]=="icon") {
				param[count][5][dpart]=text.c_str();
			}

		}
	}

	return true;
}

bool xEndElem(const std::string &name) {
	if (context[context.size()-1]=="sunr" || context[context.size()-1]=="hi" || context[context.size()-1]=="low" || context[context.size()-1]=="us" || context[context.size()-1]=="ut" || context[context.size()-1]=="dnam" || context[context.size()-1]=="suns" || context[context.size()-1]=="icon" || context[context.size()-1]=="ppcp" || context[context.size()-1]=="hmid" || context[context.size()-1]=="t" || context[context.size()-1]=="s") {
		locked= true;
	}
	if (context[context.size()-1]=="day") {
		IsDay= false;
	}

	if (context[context.size()-1]=="wind") {
		locked= true;
		IsWind=false;
	}

	if (context.size()>0) {
		context.pop_back();
	}
	return true;
}

void cxmlParse::xmlParse(int daycount, std::string DataDir, std::string data_file, int inDPart) {

	context.clear();
	//		param.clear();
	town.clear();
	unit1.clear();
	unit2.clear();

	count = 0;
	XML xml(DataDir + data_file);
	xml.nodeStartCB(xStartElem);
	xml.nodeEndCB(xEndElem);
	xml.cdataCB(xCharData);
	if (xml.parse() != 0) {
		esyslog("ERROR : weatherng: Can not parse %s\n",data_file.c_str());
	} else {
		cxmlParse::ort=town.c_str();
		cxmlParse::celsius=unit1.c_str();
		cxmlParse::speed=unit2.c_str();
		cxmlParse::sunrise=param[daycount][0][inDPart].c_str();
		cxmlParse::sunset=param[daycount][1][inDPart].c_str();
		cxmlParse::hi=param[daycount][2][inDPart].c_str();
		cxmlParse::low=param[daycount][3][inDPart].c_str();
		cxmlParse::wetter=param[daycount][4][inDPart].c_str();
		cxmlParse::icon=param[daycount][5][inDPart].c_str();
		cxmlParse::dayname=param[daycount][6][inDPart].c_str();
		cxmlParse::date=param[daycount][7][inDPart].c_str();

		cxmlParse::winddir=param[daycount][8][inDPart].c_str();
		cxmlParse::windspeed=param[daycount][9][inDPart].c_str();
		cxmlParse::humidity=param[daycount][10][inDPart].c_str();
		cxmlParse::raindown=param[daycount][11][inDPart].c_str();

	}
}

