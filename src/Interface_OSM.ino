/*
 * Project Interface_OSM
 * Description: Reception du data de l'interface NEXTION, son traitement et son affichage.
 * Author: C.G.
 * Date: 2018/1/2
 */

 STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));
 #include "Nextion.h"
 #include "SparkTime.h"
 #define dbSerial Serial

 USARTSerial& nexSerial = Serial1;
 UDP UDPClient;
 SparkTime rtc;

 unsigned long currentTime;
 unsigned long lastTime = 0UL;
 String timeStr;

 /*
  * Declare l'objet bouton [page id:0,component id:1, component name: "b0"].
  */

 NexText date0 = NexText(0, 8, "date0");

 NexButton OkB = NexButton(4, 9, "ok");  // Bouton OK de la page 4 - BRIX_SEQ
 //NexButton OkD = NexButton(5, 21, "ok"); // Bouton OK de la page 5 - DEBITS
 NexButton OkT = NexButton(6, 10, "ok"); // Bouton OK de la page 6 - TEMP_PRESS
 NexButton Ok = NexButton(7, 15, "ok"); // Bouton OK de la page 7 - SOMMAIRE OSM
 NexButton OkL = NexButton(1, 15, "ok"); // Bouton OK de la page 1 - LAVAGE
 NexButton OkR = NexButton(2, 15, "ok"); // Bouton OK de la page 2 - RINCAGE

 /* Declare les valeurs associé au lavage et rincage.
 */

 NexNumber Tlav = NexNumber(1, 1, "lav_h");
 NexNumber Trinc = NexNumber(2, 1, "rinc_h");

 NexNumber tl = NexNumber(1, 26, "tl");
 NexNumber tl_1 = NexNumber(1, 28, "tl_1");
 NexNumber tl_2 = NexNumber(1, 29, "tl_2");
 NexNumber tr = NexNumber(2, 26, "tr");
 NexNumber tr_1 = NexNumber(2, 28, "tr_1");
 NexNumber tr_2 = NexNumber(2, 29, "tr_2");

 /* Declare les valeurs associé a l'Osmose.
  */

 NexNumber DenS = NexNumber(4, 13, "val_sev_br");
 NexNumber DenC = NexNumber(4, 14, "val_con_br");
 NexDSButton SeqP = NexDSButton(4, 3, "sel_pot");
 NexText t0 = NexText(7, 19, "t0");

 NexNumber Deb1 = NexNumber(5, 1, "col1_deb");
 NexNumber Deb2 = NexNumber(5, 16, "col2_deb");
 NexNumber Deb3 = NexNumber(5, 17, "col3_deb");
 NexNumber Deb4 = NexNumber(5, 18, "col4_deb");
 NexNumber Deb5 = NexNumber(5, 19, "conc_deb");
 NexNumber Delo = NexNumber(5, 29, "delai_op");
 NexNumber Temp = NexNumber(6, 1, "val_temp");
 NexNumber Pres = NexNumber(6, 2, "val_press");

 /* Données de la page SOMMAIRE de l'Osmose.
 */
 NexNumber bs = NexNumber(7, 17, "bs");
 NexNumber bs_1 = NexNumber(7, 22, "bs_1");
 NexNumber bs_2 = NexNumber(7, 31, "bs_2");
 NexNumber bc = NexNumber(7, 2, "bc");
 NexNumber bc_1 = NexNumber(7, 23, "bc_1");
 NexNumber bc_2 = NexNumber(7, 32, "bc_2");
 NexNumber pt = NexNumber(7, 16, "pt");
 NexNumber pt_1 = NexNumber(7, 45, "pt_1");
 NexNumber pt_2 = NexNumber(7, 51, "pt_2");
 NexNumber d1 = NexNumber(7, 3, "d1");
 NexNumber d1_1 = NexNumber(7, 24, "d1_1");
 NexNumber d1_2 = NexNumber(7, 33, "d1_2");
 NexNumber d2 = NexNumber(7, 4, "d2");
 NexNumber d2_1 = NexNumber(7, 25, "d2_1");
 NexNumber d2_2 = NexNumber(7, 34, "d2_2");
 NexNumber d3 = NexNumber(7, 5, "d3");
 NexNumber d3_1 = NexNumber(7, 26, "d3_1");
 NexNumber d3_2 = NexNumber(7, 35, "d3_2");
 NexNumber d4 = NexNumber(7, 6, "d4");
 NexNumber d4_1 = NexNumber(7, 27, "d4_1");
 NexNumber d4_2 = NexNumber(7, 36, "d4_2");
 NexNumber d5 = NexNumber(7, 5, "d5");
 NexNumber d5_1 = NexNumber(7, 28, "d5_1");
 NexNumber d5_2 = NexNumber(7, 37, "d5_2");
 NexNumber hr = NexNumber(7, 1, "hr");
 NexNumber hr_1 = NexNumber(7, 52, "hr_1");
 NexNumber hr_2 = NexNumber(7, 53, "hr_2");
 NexNumber tp = NexNumber(7, 8, "tp");
 NexNumber tp_1 = NexNumber(7, 29, "tp_1");
 NexNumber tp_2 = NexNumber(7, 38, "tp_2");
 NexNumber pr = NexNumber(7, 9, "pr");
 NexNumber pr_1 = NexNumber(7, 30, "pr_1");
 NexNumber pr_2 = NexNumber(7, 39, "pr_2");

 /* Le résultat des calculs de rendement
 */
 NexNumber cc = NexNumber(7, 13, "cc");
 NexNumber cc_1 = NexNumber(7, 40, "cc_1");
 NexNumber cc_2 = NexNumber(7, 46, "cc_2");
 NexNumber deb = NexNumber(7, 12, "deb");
 NexNumber deb_1 = NexNumber(7, 41, "deb_1");
 NexNumber deb_2 = NexNumber(7, 47, "deb_2");
 NexNumber dbf = NexNumber(7, 11, "dbf");
 NexNumber dbf_1 = NexNumber(7, 42, "dbf_1");
 NexNumber dbf_2 = NexNumber(7, 48, "dbf_2");
 NexNumber dbt = NexNumber(7, 10, "dbt");
 NexNumber dbt_1 = NexNumber(7, 43, "dbt_1");
 NexNumber dbt_2 = NexNumber(7, 49, "dbt_2");

 char buffer[100] = {0};

 /*
  * Enregistre l'objet bouton a la touch liste d'evenement.
  */

 NexTouch *nex_listen_list[] =
 {
     &OkB,&OkT,&Ok,&OkL,&OkR,
     //&OkD,

     NULL
 };

  /*
  ***********************************************************************
  * Les valeurs entrees de l'OSMOSE sont traité ici.
  */
     // La composante bouton utilse la fonction pop callback.

 void OkBPopCallback(void *ptr)  // Densité et utilisation des poteaux
 {
   uint32_t densiteSev;
   uint32_t densiteConc;

   dbSerialPrintln("Bouton Densite OK");

   bs.getValue(&densiteSev);
   bc.getValue(&densiteConc);

 }

 void OkPopCallback(void *ptr)  // Les débits
 {

   uint32_t debit1;
   uint32_t debit2;
   uint32_t debit3;
   uint32_t debit4;
   uint32_t debitC;

   uint32_t resconc;
   uint32_t conheur;
   uint32_t debheur;
   uint32_t totdeb;

   uint32_t nb_poteau;

   dbSerialPrintln("Bouton Debits OK");

   //d1.setValue(debit1);
   d1.getValue(&debit1);
   //d2.setValue(debit2);
   d2.getValue(&debit2);
   //d3.setValue(debit3);
   d3.getValue(&debit3);
   //d4.setValue(debit4);
   d4.getValue(&debit4);
   //d5.setValue(debitC);
   d5.getValue(&debitC);

   pt.getValue(&nb_poteau);

 /* Affichage de la sélection de l'orientation des poteaux.
 */

   if(nb_poteau)
   {t0.setText("4-3-2-1");}
   else
   {t0.setText("1-2-3-4");}

   resconc = (((debit1 + debit2 + debit3 + debit4) * 100) / (debit1 + debit2 + debit3 + debit4 + debitC));
   cc.setValue(resconc); // Poucentage de concentré.

   conheur = (debitC * 6);
   deb.setValue(conheur); // Débit du concentré en Gallon/heure.

   debheur = ((debit1 + debit2 + debit3 + debit4) * 6);
   dbf.setValue(debheur); // Débit du filtra par heure.

   totdeb = (conheur + debheur);
   dbt.setValue(totdeb); // Débit total par heure.

 }

 void OkTPopCallback(void *ptr)  // Temperature et pression

 {

   uint32_t temp;
   uint32_t pres;

   dbSerialPrintln("Bouton Temp et Pression OK");

   tp.getValue(&temp);
   pr.getValue(&pres);

 }

 /*void OkPopCallback(void *ptr)  // Validation des données de l'Osmose au nuage
 {
 }*/

 /*
  ***********************************************************************
  * Les valeurs entrees du LAVAGE sont traité ici.
  */

 void OkLPopCallback(void *ptr)

 {
   uint32_t lavage;

   dbSerialPrintln("Bouton Lavage OK");

   tl.getValue(&lavage);

 }

 /*
  ***********************************************************************
  * Les valeurs entrees du RINCAGE sont traité ici.
  */

 void OkRPopCallback(void *ptr)
 {
   uint32_t rincage;

   dbSerialPrintln("Bouton Rincage OK");

   tr.getValue(&rincage);

 }

 void setup(void)
 {
     /* Ajuste la vitesse du port serie pour le debug et la communication avec
     l'écran Nextion. */

     nexInit();
     Serial.begin(9600);

     rtc.begin(&UDPClient, "north-america.pool.ntp.org");
     rtc.setTimeZone(-4); // emt offset

     /* Enregistre la fonction de l'évenement pop callback a la composante
     du courant bouton.*/

     OkB.attachPop(OkBPopCallback, &OkB);
     //OkD.attachPop(OkDPopCallback, &OkD);
     OkT.attachPop(OkTPopCallback, &OkT);
     Ok.attachPop(OkPopCallback, &Ok);
     OkL.attachPop(OkLPopCallback, &OkL);
     OkR.attachPop(OkRPopCallback, &OkR);

     dbSerialPrintln("setup done");
 }

 void loop(void)
 {
      /* A chaque fois qu'un évenement pop ou push est activé,
      la composante correspondant a la liste des évenements sera demandé. */
     nexLoop(nex_listen_list);

     /* Création de la date et l'heure d'affichage.
     N.B. Il y a un délai avant l'affichage.*/

     currentTime = rtc.now();
     if (currentTime != lastTime) {
       byte sec = rtc.second(currentTime);
       if (sec == 10) {
 	// Build Date String
 	timeStr = " ";
   timeStr += rtc.dayString(currentTime);
 	timeStr += "/";
   timeStr += rtc.monthString(currentTime);
 	timeStr += "/";
 	timeStr += rtc.yearString(currentTime);

   date0.setText(timeStr);}/*

 	Serial.println(timeStr);
       } else if (sec == 40) {
 	// Including current timezone
 	Serial.println(rtc.ISODateString(currentTime));
       } else if (sec == 50) {
 	// UTC or Zulu time
 	Serial.println(rtc.ISODateUTCString(currentTime));
       } else {
 	// Just the time in 12 hour format
 	timeStr = "";
 	timeStr += rtc.hour12String(currentTime);
 	timeStr += ":";
 	timeStr += rtc.minuteString(currentTime);
 	timeStr += ":";
 	timeStr += rtc.secondString(currentTime);
 	timeStr += " ";
 	timeStr += rtc.AMPMString(currentTime);
 	Serial.println(timeStr);
       }
       lastTime = currentTime;*/
     }

 }
