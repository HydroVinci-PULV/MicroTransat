/*
Antoine Raulin

Le récepteur GPS Ublox est connecté comme un périphérique SPI 0.0(/dev/spidev0.0).
Le récepteur envoie des informations par SPI dans un flux sans fin.

Compilation:
make

*/

//#define _XOPEN_SOURCE 600
#include <Common/Ublox.h>
#include <Common/Util.h>

using namespace std;

int main(int argc, char *argv[]){

    if (check_apm()) {
        return 1;
    }

    // Ce vecteur est utilisé pour stocker les données de localisation, décodées à partir des messages ubx.
    // Après avoir décodé au moins un message avec succès, les informations sont stockées dans le vecteur
    // de la manière décrite dans la fonction decodeMessage(vector<double>& data) de la classe UBXParser(voir ublox.h)
    // 
    // Pour rappel un vector est une liste de données (ici de type double) dont la taille peut évoluer.
    std::vector<double> pos_data;


    // créer une instance de classe ublox
    Ublox gps;

    // Ici, nous testons la connexion avec le récepteur. La fonction testConnection() attend un message du protocole ubx et le vérifie.
    // S'il y a au moins un message correct dans les 300 premiers symboles, le test est réussi.
    if(gps.testConnection())
    {
        printf("Ublox test OK\n");
        if (!gps.configureSolutionRate(1000))
        {
            printf("Setting new rate: FAILED\n");
        }


        // Here, however we use a different approach. Instead of trying to extract info from every message(as done in decodeMessages()),
        // this function waits for a message of a specified type and gets you just the information you need
        // In this example we decode NAV_STATUS and NAV-POSLLH messages, adding new types, however is quite easy

        // On attend un message d'un type spécifique
        // Ici on va décoder les messages NAV_STATUS et NAV_POSLLH
        while (true)
        {

            // On décode le message NAV_POSLLH et on récupère les informations dans le vecteur pos_data
            if (gps.decodeSingleMessage(Ublox::NAV_POSLLH, pos_data) == 1)
            {

                // Petit récap sur comment fonctionne un gps :
                // Le système temporel utilisé par les GPS fonctionne comme suit :
                // Il nous envoit le nombres de semaines passées depuis minuit le 6 janvier 1980
                // ainsi que les millisecondes écoulées depuis le début de la dernière semaine.

                // Il faudra vérifier si il s'agit bien ici des millisecondes depuis le début de la semaine GPS
                // Même si ca nous est pas utile pour la géolocalisation, il est utile pour le calcul de la date pour la rasp et pour la télémetrie.
                printf("GPS Millisecond Time of Week: %.0lf s\n", pos_data[0]/1000);
                // Position dans l'espace sur le plan terrestre
                printf("Longitude: %lf\n", pos_data[1]/10000000);
                printf("Latitude: %lf\n", pos_data[2]/10000000);
                // TODO : améliorer les recherches si il s'agit du positionnement du satellite gps ou pas
                printf("Height above Ellipsoid: %.3lf m\n", pos_data[3]/1000);
                // hauteur par rapport au niveau de la mer, qu'on pourra combiner avec les données du baromètre.
                printf("Height above mean sea level: %.3lf m\n", pos_data[4]/1000);
                // et les précisions sur la position
                printf("Horizontal Accuracy Estateimate: %.3lf m\n", pos_data[5]/1000);
                printf("Vertical Accuracy Estateimate: %.3lf m\n", pos_data[6]/1000);

            } 

            // On décode le message NAV_STATUS et on récupère les informations dans le vecteur pos_data
            // NAV_STATUS précise si la connexion est active avec le satellite ou les autres types d'erreurs
            if (gps.decodeSingleMessage(Ublox::NAV_STATUS, pos_data) == 1)
            {
                printf("Current GPS status:\n");
                printf("gpsFixOk: %d\n", ((int)pos_data[1] & 0x01));

                printf("gps Fix status: ");
                switch((int)pos_data[0]){
                    case 0x00:
                        printf("no fix\n");
                        break;

                    case 0x01:
                        printf("dead reckoning only\n");
                        break;

                    case 0x02:
                        printf("2D-fix\n");
                        break;

                    case 0x03:
                        printf("3D-fix\n");
                        break;

                    case 0x04:
                        printf("GPS + dead reckoning combined\n");
                        break;

                    case 0x05:
                        printf("Time only fix\n");
                        break;

                    default:
                        printf("Reserved value. Current state unknown\n");
                        break;

                }

                printf("\n");

            }

            // Petite pause de 200 micro-secondes
            usleep(200);
        }

    } else {
        // Les tests n'ont pas réussi
        printf("Ublox test not passed\nAbort program!\n");

    }

    return 0;
}
