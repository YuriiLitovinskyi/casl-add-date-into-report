#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>


// Current program will actualize CASL Cloud database file, which leads to display data in a report called "Lost connection devices"

int main(int argc, char *argv[]) {
	
    sqlite3 *db;
    char *err_msg = 0;  
    
    // Open CASL DB
    int rc = sqlite3_open("data.db", &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open CASL Cloud database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        system("pause");
        
        return 1;
    }    
    
    // query                 
    char sql1[1500] = "UPDATE device SET enabled = IFNULL((SELECT enabled FROM (SELECT max(time), enabled, device_id " \
                        "FROM (SELECT event.time, event.device_id, (CASE WHEN event.dict_event_type IN (0x3f15, 0x3f95) THEN event.time " \
                        "ELSE -event.time END) " \
                        "AS enabled FROM device, event WHERE device.device_id = event.device_id " \
                        "AND event.dict_event_type IN (0x3f15,0x3f95, 0x3f16, 0x3f96)) " \
                        "GROUP BY device_id) " \
                        "WHERE device_id = device.device_id), -1), no_power=IFNULL((SELECT isNoPower " \
	                    "FROM (SELECT max(time), isNoPower, device_id FROM (SELECT event.time, event.device_id, " \
	                    "(CASE WHEN event.dict_event_type IN (0x0068, 0x006b, 0x0168) THEN event.time ELSE -event.time END) AS " \
	                    "isNoPower FROM device, event WHERE device.device_id = event.device_id AND event.dict_event_type IN " \
	                    "(0x0069, 0x006a, 0x0068, 0x006b, 0x0168, 0x0169)) " \
	                    "GROUP BY device_id) WHERE device_id = device.device_id), 0), offline=IFNULL((SELECT offline FROM (" \
	                    "SELECT max(event.time), event.device_id, CASE " \
	                    "WHEN event.dict_event_type IN (0x0061, 0x6100, 0x0161, 0x0162) THEN event.time " \
                        "ELSE -event.time END AS offline FROM device, event " \
                        "WHERE device.device_id = event.device_id AND time > 0 " \
                        "GROUP BY event.device_id) WHERE device_id = device.device_id), 1);";

	//printf(sql1);	        
    
    rc = sqlite3_exec(db, sql1, 0, 0, &err_msg);           
     
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        system("pause");
        
        return 1;
    }    
    
    sqlite3_close(db);  
    
    printf("\nChanges to DB were successfully applied! \n\n");    
        
    system("pause");   
       
	return 0;
}
