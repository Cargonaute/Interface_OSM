/*
 * Project Interface_OSM
 * Description: Reception du data de l'interface NEXTION, son traitement et son affichage.
 * Author: C.G.
 * Date: 2018/1/2
 */

 STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));
 #include "Nextion.h"
 #define dbSerial Serial
 #define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
 unsigned long lastSync = millis();

 USARTSerial& nexSerial = Serial1;

 unsigned long currentTime;
 unsigned long lastTime = 0UL;
 char timeStr[20];

 // Variable relié à l'opération du buffer circulaire
 const int buffSize = 4; // Nombre max d'événements que l'on peut sauvegarder
 struct Sommaire{
   uint32_t debit1 = 0;
   uint32_t debit2 = 0;
   uint32_t debit3 = 0;
   uint32_t debit4 = 0;
   uint32_t debitC = 0;

   uint32_t resconc = 0;
   uint32_t conheur = 0;
   uint32_t debheur = 0;
   uint32_t totdeb = 0;

   uint32_t nb_poteau = 0; // Ordre de sequence d'osmose

   uint32_t densiteSev = 0;
   uint32_t densiteConc = 0;

   uint32_t temp = 0;
   uint32_t pres = 0;
 };

struct Sommaire SC; // Sommaire Courant.
struct Sommaire Som_hist1;  // Sommaire premiere ligne historique.
struct Sommaire Som_hist2;  // Sommaire seconde ligne historique.
struct Sommaire Som_hist3;  // Sommaire troisieme ligne historique.

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
 NexButton OkHist = NexButton(7, 18, "OkHist"); // Bouton mise en historique.

 /* Declare les valeurs associé au lavage et rincage.
 */

 NexNumber Tlav = NexNumber(1, 1, "lav_h");
 NexNumber Trinc = NexNumber(2, 1, "rinc_h");

 NexNumber tl = NexNumber(1, 26, "tl");
 NexNumber tr = NexNumber(2, 26, "tr");


 /* Declare les valeurs associé a l'Osmose.
  */

 NexNumber DenS = NexNumber(4, 13, "val_sev_br");
 NexNumber DenC = NexNumber(4, 14, "val_con_br");
 NexDSButton SeqP = NexDSButton(4, 3, "sel_pot");
 NexText t0 = NexText(7, 19, "t0");
 NexText hist_1 = NexText(7, 22, "hist_1");
 NexText hist_2 = NexText(7, 23, "hist_2");

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
 NexNumber bc = NexNumber(7, 2, "bc");
 NexNumber pt = NexNumber(7, 16, "pt");
 NexNumber d1 = NexNumber(7, 3, "d1");
 NexNumber d2 = NexNumber(7, 4, "d2");
 NexNumber d3 = NexNumber(7, 5, "d3");
 NexNumber d4 = NexNumber(7, 6, "d4");
 NexNumber d5 = NexNumber(7, 5, "d5");
 NexNumber hr = NexNumber(7, 1, "hr");
 NexNumber tp = NexNumber(7, 8, "tp");
 NexNumber pr = NexNumber(7, 9, "pr");

 NexText Som_1 = NexText(7, 25, "Som_1");
 NexText Som_2 = NexText(7, 26, "Som_2");

 /* Le résultat des calculs de rendement
 */
 NexNumber cc = NexNumber(7, 13, "cc");
 NexNumber deb = NexNumber(7, 12, "deb");
 NexNumber dbf = NexNumber(7, 11, "dbf");
 NexNumber dbt = NexNumber(7, 10, "dbt");


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

   Serial.println("Bouton Densite OK");

   bs.getValue(&SC.densiteSev);
   bc.getValue(&SC.densiteConc);

 }

void OkHistPopCallback(void *ptr)
{
  String affHist1;
  char affHist1_L1[40];
  char affHist1_L2[40];
  char affHist1_L3[40];
  char affHist2;
  Som_hist3 = Som_hist2;
  Som_hist2 = Som_hist1;
  Som_hist1 = SC;
  sprintf(affHist1_L1, "%2d %2d  %4d %4d %4d %4d %4d  %3d %3d  %2d", Som_hist1.densiteSev,Som_hist1.densiteConc,Som_hist1.debit1,Som_hist1.debit2,Som_hist1.debit3,Som_hist1.debit4,Som_hist1.debitC,Som_hist1.temp,Som_hist1.pres);
  // sprintf(affHist1_L1, "%2d", Som_hist1.densiteSev);
  sprintf(affHist1_L2, "%2d %2d  %4d %4d %4d %4d %4d  %3d %3d  %2d", Som_hist2.densiteSev,Som_hist2.densiteConc,Som_hist2.debit1,Som_hist2.debit2,Som_hist2.debit3,Som_hist2.debit4,Som_hist2.debitC,Som_hist2.temp,Som_hist2.pres);
  affHist1 = String(affHist1) + String(affHist1_L2);

  hist_1.setText(affHist1);

  Serial.println(affHist1);
  // pense a delo!!!!!!
  // sprintf(affHist2, "",Som_hist1.resconc,Som_hist1.conheur,Som_hist1.debheur,Som_hist1.totdeb); //Hist1.nb_poteau
}

 void OkPopCallback(void *ptr)  // Les débits
 {
   Serial.println("Bouton Debits OK");

   //d1.setValue(debit1);
   d1.getValue(&SC.debit1);
   //d2.setValue(debit2);
   d2.getValue(&SC.debit2);
   //d3.setValue(debit3);
   d3.getValue(&SC.debit3);
   //d4.setValue(debit4);
   d4.getValue(&SC.debit4);
   //d5.setValue(debitC);
   d5.getValue(&SC.debitC);

   pt.getValue(&SC.nb_poteau);



 /* Affichage de la sélection de l'orientation des poteaux.
 */

   if(SC.nb_poteau)
     {t0.setText("4-3-2-1");}
   else
     {t0.setText("1-2-3-4");}

     SC.resconc = (((SC.debit1 + SC.debit2 + SC.debit3 + SC.debit4) * 100) /
                (SC.debit1 + SC.debit2 + SC.debit3 + SC.debit4 + SC.debitC));
     cc.setValue(SC.resconc); // Poucentage de concentré.

     SC.conheur = (SC.debitC * 6);
     deb.setValue(SC.conheur); // Débit du concentré en Gallon/heure.

     SC.debheur = ((SC.debit1 +SC.debit2 + SC.debit3 + SC.debit4) * 6);
     dbf.setValue(SC.debheur); // Débit du filtra par heure.

     SC.totdeb = (SC.conheur + SC.debheur);
     dbt.setValue(SC.totdeb); // Débit total par heure.

     Particle.publish("Data_OSM", "Dummy data", 60, PRIVATE);
 }


 void OkTPopCallback(void *ptr)  // Temperature et pression

 {

   Serial.println("Bouton Temp et Pression OK");

   tp.getValue(&SC.temp);
   pr.getValue(&SC.pres);

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

   Serial.println("Bouton Lavage OK");

   tl.getValue(&lavage);

 }

 /*
  ***********************************************************************
  * Les valeurs entrees du RINCAGE sont traité ici.
  */

 void OkRPopCallback(void *ptr)
 {
   uint32_t rincage;

   Serial.println("Bouton Rincage OK");

   tr.getValue(&rincage);

 }

 void setup(void)
 {
     /* Ajuste la vitesse du port serie pour le debug et la communication avec
     l'écran Nextion. */

     nexInit();
     //dbSerialBegin(9600);
     dbSerialBegin(9600);

     //rtc.begin(&UDPClient, "north-america.pool.ntp.org");
     //rtc.setTimeZone(-4); // emt offset
     Time.zone(-4);

     /* Enregistre la fonction de l'évenement pop callback a la composante
     du courant bouton.*/

     OkB.attachPop(OkBPopCallback, &OkB);
     //OkD.attachPop(OkDPopCallback, &OkD);
     OkT.attachPop(OkTPopCallback, &OkT);
     Ok.attachPop(OkPopCallback, &Ok);
     OkL.attachPop(OkLPopCallback, &OkL);
     OkR.attachPop(OkRPopCallback, &OkR);
     OkHist.attachPop(OkHistPopCallback, &OkHist);

     Serial.println("setup done");
 }

 void loop(void)
 {
   if (millis() - lastSync > ONE_DAY_MILLIS) {
     // Request time synchronization from the Particle Cloud
     time_t lastSyncTimestamp;
     unsigned long lastSync = Particle.timeSyncedLast(lastSyncTimestamp);
     if (millis() - lastSync > ONE_DAY_MILLIS) {
       unsigned long cur = millis();
       Serial.printlnf("Time was last synchronized %lu milliseconds ago", millis() - lastSync);
       if (lastSyncTimestamp > 0)
       {
         Serial.print("Time received from Particle Cloud was: ");
         Serial.println(Time.timeStr(lastSyncTimestamp));
       }
       // Request time synchronization from Particle Cloud
       Particle.syncTime();
       // Wait until Photon receives time from Particle Cloud (or connection to Particle Cloud is lost)
       waitUntil(Particle.syncTimeDone);
       // Check if synchronized successfully
       if (Particle.timeSyncedLast() >= cur)
       {
         // Print current time
         Serial.println(Time.timeStr());
       }
     }   }
    /* A chaque fois qu'un évenement pop ou push est activé,
    la composante correspondant a la liste des évenements sera demandé. */

   nexLoop(nex_listen_list);

   /* Création de la date et l'heure d'affichage.
   N.B. Il y a un délai avant l'affichage.*/

   currentTime = Time.now();
   if (currentTime != lastTime) {

   	// Build Date String
    sprintf(timeStr, "% 4d/%02d/%02d %02d:%02d:%02d", Time.year(currentTime), Time.month(currentTime), Time.day(currentTime),
    Time.hour(currentTime), Time.minute(currentTime), Time.second(currentTime));
    date0.setText(timeStr);
    Serial.println(timeStr);
    lastTime = currentTime;
  }

 }
