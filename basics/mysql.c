/*
gcc test.c -o test `mysql_config --libs`
./test
mysql_config --cflags


sudo apt-get install mysql-server
*/


#include<stdio.h>
#include <mysql/mysql.h>

int main(int argc, char **argv)
{  
  	MYSQL *con = mysql_init(NULL);
	    if (con == NULL)
	    {
		    fprintf(stderr, "%s\n", mysql_error(con));
		    exit(1);
	    }
	    if (mysql_real_connect(con, "localhost", "root", "yogita1996", NULL, 0, NULL, 0) == NULL) 
	    {
		    fprintf(stderr, "%s\n", mysql_error(con));
		    mysql_close(con);
		    exit(1);
	    } 
	    if (mysql_query(con, "USE testdb")) 
	    {
		    fprintf(stderr, "%s\n", mysql_error(con));
		    mysql_close(con);
		    exit(1);
	    }
const char *query = "SELECT * FROM candidate";

	    if (mysql_query(con, query) != 0)
	   {
    		fprintf(stderr, "%s\n", mysql_error(con));
    		exit(-1);
  		} 
	else {
    		MYSQL_RES *query_results = mysql_store_result(con);
    		if (query_results)
		 { // make sure there *are* results..
      		MYSQL_ROW row;

      		while((row = mysql_fetch_row(query_results)) !=0)
      		{
        /* Since your query only requests one column, I'm
         * just using 'row[0]' to get the first field. */

        /* Set a float 'f' to the value in 'row[0]', or
         * 0.0f if it's NULL */
        //	float f = row[0] ? atof(row[0]) : 0.0f;

        /* Do whatever you need to with 'f' */
        	printf("%s\t%s\n", row[0],row[1]);
      		}
	}
	}
	
	    mysql_close(con);
	    exit(0);
	}
