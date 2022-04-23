UPDATE device SET
    enabled = IFNULL((SELECT enabled
                          FROM (
                            SELECT
                                max(time), enabled, device_id
                            FROM (
                                SELECT event.time, event.device_id,
                                    (CASE
                                        WHEN event.dict_event_type IN (0x3f15, 0x3f95) THEN event.time
                                        ELSE -event.time
                                    END) AS enabled
                                FROM device, event
                                WHERE
                                    device.device_id = event.device_id
                                    AND event.dict_event_type IN (0x3f15,0x3f95, 0x3f16, 0x3f96)
                            )
                            GROUP BY device_id
                          )
                          WHERE device_id = device.device_id), -1),
    no_power=IFNULL((SELECT isNoPower
                          FROM (
              SELECT
                  max(time), isNoPower, device_id
              FROM (
                  SELECT event.time, event.device_id,
                      (CASE WHEN event.dict_event_type IN (0x0068, 0x006b, 0x0168) THEN event.time ELSE -event.time END) AS isNoPower
                  FROM device, event
                  WHERE
                      device.device_id = event.device_id
                      AND event.dict_event_type IN (0x0069, 0x006a, 0x0068, 0x006b, 0x0168, 0x0169)
              )
              GROUP BY device_id
          )
                          WHERE device_id = device.device_id), 0),
    offline=IFNULL((SELECT offline
                          FROM (
              SELECT max(event.time), event.device_id,
                  CASE
                      WHEN event.dict_event_type IN (0x0061, 0x6100, 0x0161, 0x0162) THEN event.time
                      ELSE -event.time
                  END AS offline
              FROM device, event
              WHERE
                  device.device_id = event.device_id
                  AND time > 0
              GROUP BY
                  event.device_id
          )
                          WHERE device_id = device.device_id), 1);