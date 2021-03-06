/*
* PAM_PTEIDCC.c
*
* Description: This dinamic library uses the Portuguese Citizen
*              Card to authenticate a user already registered
* Usage: auth sufficient pam_PTEIDCC.so [public key file]
* Author: André Zúquete (http://www.ieeta.pt/~avz)
* Date: May 2009
*/

// #define PAM_DEBUG

#include <sys/param.h>

#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <memory.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <unistd.h>

#define PAM_SM_AUTH
#define PAM_SM_ACCOUNT
#define	PAM_SM_PASSWORD

#include <security/pam_modules.h>
#include <security/pam_client.h>
#include <security/_pam_macros.h>
#include <security/pam_appl.h>

#include "pteidlib.h"
#include "cryptoki.h"
#include <openssl/x509.h>

#include "CCkpubFile.h"

static char * command;

/*
* Generic function that finds a PKCS#11 object, given its class and
* label, in a crypto token
*/



/*
* Check if a given public key exists in a certificate inside the
* crypto token
* If present, use the corresponding private key to encrypt a
* challenge and decrypt the result with the public key
*/

int calcDecodeLength(const char* b64input) { //Calculates the length of a decoded base64 string
  int len = strlen(b64input);
  int padding = 0;
 
  if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
    padding = 2;
  else if (b64input[len-1] == '=') //last char is =
    padding = 1;
 
  return (int)len*0.75 - padding;
}


 
int Base64Decode(char* b64message, char** buffer) { //Decodes a base64 encoded string
  BIO *bio, *b64;
  int decodeLen = calcDecodeLength(b64message),
      len = 0;
  *buffer = (char*)malloc(decodeLen+1);
  FILE* stream = fmemopen(b64message, strlen(b64message), "r");
 
  b64 = BIO_new(BIO_f_base64());
  bio = BIO_new_fp(stream, BIO_NOCLOSE);
  bio = BIO_push(b64, bio);
  len = BIO_read(bio, *buffer, strlen(b64message));
    //Can test here if len == decodeLen - if not, then return an error
  (*buffer)[len] = '\0';
 
  BIO_free_all(bio);
  fclose(stream);
 
  return (0); //success
}

char *unbase64(unsigned char *input, int length)
{
  BIO *b64, *bmem;

  char *buffer = (char *)malloc(length);
  memset(buffer, 0, length);

  b64 = BIO_new(BIO_f_base64());
  bmem = BIO_new_mem_buf(input, length);
  bmem = BIO_push(b64, bmem);

  BIO_read(bmem, buffer, length);

  BIO_free_all(bmem);

  return buffer;
}


/*
* Authentication management
*/




PAM_EXTERN int
pam_sm_authenticate ( pam_handle_t *pamh, int flags, int argc,
			const char *argv[] )
{
	/*struct passwd *pwd;
	int retval;
	const char *user, *realpw, *prompt;

    printf("adasdasdas\n");


	retval = pam_get_user ( pamh, &user, NULL );
	if (retval != PAM_SUCCESS)
		return retval;
	pwd = getpwnam ( user );

	D(("Got user: %s", user));

	if (pwd == NULL) {
	    return PAM_AUTH_ERR;
	}

	return CC_login ( pamh, pwd, (argc >= 1) ? argv[0] : CC_KPUB_FILE );*/

    //RSA * key;



    /*if (RSA_verify ( NID_sha1, "digest", 20, "asdasd", 128, key ) == 1) {
        D(("PTEID CC authentication: success!"));
        return PAM_SUCCESS;
    }*/
    int retval;
    const char *user;
    const char *userHash;
    const char *bdHash;
    // int fd;
    //unsigned char challenge[64];
    //unsigned char digestToSign[20];


    struct pam_response *response;
    //response = (pam_response *) malloc(200)
    //response->resp = (char *) malloc(150);

    //fd = open ( "/dev/urandom", O_RDONLY );
    //read ( fd, challenge, sizeof(challenge) );
    //close ( fd );

    //SHA1_Init ( &ctx );
    //SHA1_Update ( &ctx, challenge, sizeof(digestToSign) );
    //SHA1_Final ( digestToSign, &ctx );

    
    retval = pam_get_item(pamh, PAM_USER, (const void **) &user);

    pam_prompt(pamh, PAM_PROMPT_ECHO_OFF, response, user);

    if(strlen( response->resp ) != 48){
      //printf("PAM_AUTH_ERR\n");
      return PAM_AUTH_ERR;
    }

    bdHash = (char *) malloc (48);
    strcpy (bdHash, response->resp);
    free(response->resp);

    pam_prompt(pamh, PAM_PROMPT_ECHO_ON, response, user);
    if(strlen(response->resp) != 48){
      return PAM_AUTH_ERR;
    }

    userHash = (char *) malloc (48);

    strcpy (userHash, response->resp);
    free(response->resp);

    if(strcmp(userHash, bdHash) == 0){
      return PAM_SUCCESS;
    }

    return PAM_AUTH_ERR;

    
}

/*
* Credentials management
*/

PAM_EXTERN int
pam_sm_setcred ( pam_handle_t *pamh, int flags, int argc,
			const char *argv[] )
{
    return PAM_SUCCESS;
}

/*
* Account management
*/

pam_sm_acct_mgmt ( pam_handle_t *pamh, int flags, int argc,
			const char *argv[] )
{
    return PAM_SUCCESS;
}

/*
* Password management
*/

PAM_EXTERN int
pam_sm_chauthtok ( pam_handle_t *pamh, int flags, int argc,
			const char *argv[] )
{
    return (PAM_SERVICE_ERR);
}

#ifdef PAM_MODULE_ENTRY
PAM_MODULE_ENTRY("pam_pw");
#endif
