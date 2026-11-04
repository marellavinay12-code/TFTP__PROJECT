
#include "head.h"

int main()
{
    int choice;
    char mode[16] = "";

    tftp_packet opcode_information;

    do
    {
        printf("\nMenu\n");
        printf("1. Connect\n");
        printf("2. Put (Send file)\n");
        printf("3. Get (Receive file)\n");
        printf("4. Mode (Select transfer mode)\n");
        printf("5. Exit\n");

        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input\n");
            return 1;
        }

        switch (choice)
        {
            case 1:
                connect_server(&opcode_information);
                break;

            case 2:
                if (server_fd < 0)
                {
                    printf("Please start the server and connect first\n");
                    break;
                }

                if (mode[0] == '\0')
                {
                    printf("Please select transfer mode first\n");
                    break;
                }

                put_data(&opcode_information, mode);
                break;

            case 3:
                if (server_fd < 0)
                {
                    printf("Please start the server and connect first\n");
                    break;
                }

                if (mode[0] == '\0')
                {
                    printf("Please select transfer mode first\n");
                    break;
                }

                get_data(&opcode_information, mode);
                break;

            case 4:
                select_mode(mode);
                break;

            case 5:
                printf("Exit\n");
                break;

            default:
                printf("Invalid choice\n");
                break;
        }

    } while (choice != 5);

    return 0;
}
