#include <winsock.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

typedef struct sockaddr_in sockaddr_in;

static int print_usage()
{
    fprintf(stdout, "%s\n", "USAGE: wol <mac_address> [<broadcast_address>]");
    return 1;
}

static void create_magic_packet(unsigned char *packet, unsigned int *macAddress){
	int i;
	unsigned int mac[6];

	for(i = 0; i < 6; i++)
    {
		packet[i] = 0xFF;
		mac[i] = macAddress[i];
	}
	for(i = 1; i <= 16; i++)
    {
		memcpy(&packet[i * 6], &mac, 6 * sizeof(unsigned char));
	}
}

int main(int argsCount, char **args)
{
    if (argsCount < 2)
    {
        return print_usage();
    }
    else
    {
        unsigned int mac[6] = { 0 };
        char broadcast_address[] = "255.255.255.255";
        unsigned char packet[102] = { 0 };

        int mac_read = sscanf_s(
            args[1],
            "%x:%x:%x:%x:%x:%x",
            &(mac[0]),
            &(mac[1]),
            &(mac[2]),
            &(mac[3]),
            &(mac[4]),
            &(mac[5]));

        if (mac_read != 6)
        {
            fprintf(stderr, "[ERROR] %s\n", "Invalid MAC address, format is XX:XX:XX:XX:XX:XX.");
            return print_usage();
        }

        fprintf(
            stdout,
            "Sending packet to %02x::%02x:%02x:%02x:%02x:%02x.\n",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        if (argsCount > 2)
        {
            int broadcast_address_read = sscanf_s(
                args[2],
                "%d.%d.%d.%d",
                &broadcast_address_read, &broadcast_address_read, &broadcast_address_read, &broadcast_address_read);
            if (broadcast_address_read == 4)
            {
                strncpy_s(broadcast_address, 102, args[2], sizeof(broadcast_address) - 1);
            }
            fprintf(stdout, "Broadcast address %s will be used.\n", broadcast_address);
        }

        create_magic_packet(packet, mac);

        sockaddr_in udpClient = { 0 };
        WSADATA winsockDesc = { 0 };
        if (!WSAStartup(MAKEWORD(2, 2), &winsockDesc))
        {
            SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            int broadcast = 1;
            if(setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, (char *) &broadcast, sizeof(broadcast)) == -1){
                fprintf(stderr, "[ERROR] %s (%d)\n", "Failed to setup a broadcast socket.", WSAGetLastError());
                WSACleanup();
                exit(EXIT_FAILURE);
            }
            sockaddr_in udpClient =
            {
                .sin_family = AF_INET,
                .sin_addr.s_addr = INADDR_ANY,
                .sin_port = htons(0),
            };

            int status = bind(udpSocket, (struct sockaddr*) &udpClient, sizeof(udpClient));
            if (status)
            {
                fprintf(stderr, "[ERROR] Could not bind udp socket.\n");
                return 1;
            }

            sockaddr_in udpServer =
            {
                .sin_family = AF_INET,
                .sin_addr.s_addr = inet_addr(broadcast_address),
                .sin_port = htons(9),
            };

            status = sendto(
                udpSocket,
                (char*)&packet,
                sizeof(unsigned char) * 102,
                0,
                (struct sockaddr*) &udpServer,
                sizeof(udpServer));
            if (!status)
            {
                fprintf(stderr, "[ERROR] Could not send packet to client.\n");
                return 1;
            }
        }
        fprintf(stdout, "%s\n", "Wake up magic packet was sent.");
    }
    return 0;
}