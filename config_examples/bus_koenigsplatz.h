//Bus-Verbindung Linie 58, 68 und 100 am Königsplatz

static const Config configs[] = {
    {
        /* wifi_name           */ "wifi_test",
        /* wifi_pass           */ "wifi_pw",
        /* type                */ mvg_api,
        /* url                 */ "de:09162:110",
	      /* include_type        */ {"BUS" },
        /* include_line        */ { "*" },
        /* exclude_destination */ {},
    },


//Beispiel nur Bus 68 und 100 (ohne Bus-Linie 58)
    // {
    //     /* wifi_name           */ "wifi_test",
    //     /* wifi_pass           */ "wifi_pw",
    //     /* type                */ mvg_api,
    //     /*url                  */"de:09162:110",
	  //     /* include_type        */ {"BUS" },
    //     /* include_line        */ {"68","100"},
    //     /* exclude_destination */ {},
    // },
};

