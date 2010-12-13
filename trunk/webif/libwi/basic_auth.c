#include <klone/utils.h>
#include "basic_auth.h"
#include "conf.h"

bool isAuthorized(wcontext_t *wctx){
	const char *valeur=NULL;
	int i;
	char *decode;
	bool result=false;
	char *user=NULL;
	char *password=NULL;
	
	if (!webifConf.user || !webifConf.user[0] || !webifConf.password ||  !webifConf.password[0]){
		//authorization not requiered
		return true;
	}
	
	//get login and password
	valeur = request_get_field_value(wctx->request,"Authorization");
	
	if (valeur!=NULL) {
		//remove the "Basic " word of base64 string
		valeur += 6;

		decode = base64decode(valeur);
		
		//distinguish the login and the password
		char *p;
		for (i=0,p=strtok(decode,":");p!=0 && i<3;p=strtok(0,":"),i++) {
			switch (i){
				case 0: user=p; break;
				case 1: password=p; break;
				default: 
					warn("i==2");
					break;
			}
		}
		//compare the login with a static string
		if (user && password){
			if (strcmp(webifConf.user,user)==0 && strcmp(webifConf.password,password)==0){
				result=true;
			}
		}
		free(decode);
	}
	if (!result){
		//the user is refused
		response_set_status(wctx->response, HTTP_STATUS_UNAUTHORIZED);
		response_set_field(wctx->response,"WWW-Authenticate", "Basic realm=\"Identification\"");
		response_set_field(wctx->response,"Cache-Control","no-cache");
		response_set_field(wctx->response,"Connection","close");
		//HTML page for the client
		io_printf(wctx->out,"<html><head><title>Autentificación fallida</title></head><body><p class=\"alert\">No autorizado</p></body></html>"); //TODO i18n
	}
	return result;
}

char *base64decode(const char *encodedstring){
	char inalphabet[256], decoder[256];
	unsigned char alphabet[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int i;
	int bits, c;
	int char_count;
	int errors = 0;//number of found error
	char *res;//decoded string "login:mdp\0"
	char *ptr=NULL;

	//initialisation of arrays
	memset(inalphabet,0,256*sizeof(char));
	memset(decoder,0,256*sizeof(char));
	i=strlen(encodedstring);
	res = (char *)malloc(i*sizeof(char));
	memset(res,0,i*sizeof(char));

	for(i = (sizeof alphabet) - 1; i >= 0 ; i--)
	{
		inalphabet[alphabet[i]] = 1;
		decoder[alphabet[i]] = i;
	}

	char_count = 0;
	bits = 0;
	ptr = (char *)encodedstring;
	i=0;
	while((c = *(ptr++)) != '\0') {
		if(c == '=')
			break;
		if(c > 255 || !inalphabet[c])
			continue;
		bits += decoder[c];
		char_count++;
		if(char_count == 4) {
			res[i++]=((bits >> 16));
			res[i++]=(((bits >> 8) & 0xff));
			res[i++]=((bits & 0xff));
			bits = 0;
			char_count = 0;
		} else {
			bits <<= 6;
		}
	}
	if(c == '\0') {
		if(char_count) {
			//base64 encoding incomplete: at least ((4 - char_count) * 6)) bits truncated
			errors++;
		}
	} else { /* c == '=' */
		switch (char_count) {
			case 1:
				//base64 encoding incomplete: at least 2 bits missing
				errors++;
				break;
			case 2:
				res[i++]=((bits >> 10));
				break;
			case 3:
				res[i++]=((bits >> 16));
				res[i++]=(((bits >> 8) & 0xff));
				break;
		}
	}
	res[i]='\0';
	//no error
	if(errors==0)
		return res;
	
	//found error
	free(res);
	return NULL;
}
