#include <windows.h>
#include <iostream>
#include <climits>
#include <sstream>
#include <string.h>
#include <map>
using namespace std;
char* toHex(int t){
    char* buf = new char[4];
    buf[0] = (t & 0x000000FF);
    buf[1] = (t & 0x0000FF00) >> 8;
    buf[2] = (t & 0x00FF0000) >> 16;
    buf[3] = (t & 0xFF000000) >> 24;
 
    return buf;
}
 
void toOffset(char*& buf, char* t, int offset){
    for(int i = 0; i < 4; ++i){
        buf[offset + i] = t[i];
    }
}
void getOffset(char*& buf, char* t, int offset){
    for(int i = offset + 4 - 1; i >= offset ; --i)
        buf[i -  offset] = t[i];
}
 
int toInt(char* buf){
    return *(int*)buf;
}
 
bool ReadSector(const char* dsk, char *& buf, int nS, unsigned int numSect){
    DWORD dwRead;
    HANDLE hDisk = CreateFile(dsk, GENERIC_READ, FILE_SHARE_VALID_FLAGS, 0, OPEN_EXISTING, 0, 0);
    if(hDisk == INVALID_HANDLE_VALUE){//Check create file success or not
        CloseHandle(hDisk);
        return 0;
    }
    SetFilePointer(hDisk, numSect * 512, 0, FILE_BEGIN);//Move file pointer to sector n = numSect * 512
 
    ReadFile(hDisk, buf, nS * 512, &dwRead, 0);//Read nS * 512 byte at sector n = numSect * 512
    SetFilePointer(hDisk, 0, 0, FILE_BEGIN);
    CloseHandle(hDisk);
    return 1;
}
 
bool WriteSector(char* dsk, const char* buf, int nS, unsigned int numSect){
    DWORD dwWrite;
    HANDLE hDisk = CreateFile(dsk, GENERIC_WRITE, FILE_SHARE_VALID_FLAGS, 0, OPEN_EXISTING, 0, 0);
    if(hDisk == INVALID_HANDLE_VALUE){//Check create file success or not
        CloseHandle(hDisk);
        return 0;
    }
    SetFilePointer(hDisk, numSect * 512, 0, FILE_BEGIN);
    WriteFile(hDisk, buf, 512 * nS, &dwWrite, 0);//Write 512 * bS byte to sector
    SetFilePointer(hDisk, 0, 0, FILE_BEGIN);
    CloseHandle(hDisk);
    return 1;
}
 
pair<bool, int> readInt(char* dsk, int numS, int offset){
    if(offset < 0 || offset > 508)
        return make_pair(false, -1);
    int res = 0;
    char* buf = new char[512];
    bool ok = ReadSector(dsk, buf, 1, numS);
    char* r = new char[4];
    getOffset(r, buf, offset);
    res = toInt(r);
    delete[] buf;
    delete[] r;
    return make_pair(ok, res);
}
 
bool writeInt(char* dsk, int numS, int num, int offset){
    char* buf = new char[512];
    char* t = toHex(num);
    if(offset < 0 || offset > 508)
        return 0;
    bool ok1 = ReadSector(dsk, buf, 1, numS);
    toOffset(buf, t, offset);
    bool ok = WriteSector(dsk, buf, 1, numS);
    delete[] buf;
    delete[] t;
    return ok & ok1;
}
 
int main(){
    map<char, char*> m;
    m['c'] = "\\\\.\\C:";
    m['d'] = "\\\\.\\D:";
    m['e'] = "\\\\.\\E:";
    m['f'] = "\\\\.\\F:";
    char* buf = new char[512];
    cout << "Choose letter to write sector: \n";
    cout << "Support: c, d, e, f\n";
    cout << "Your choice: ";
    char choice;
    cin >> choice;
    char* dsk = m[choice];
    cout << "You are working at: " << dsk << '\n';
    while(1){
        cout << "1. Write integer to sector\n";
        cout << "2. Read integer from sector\n";
        cout << "3. Write sector\n";
        cout << "4. Read sector\n";
        cout << "0. Exit!\n";
        cout << "Input your choice: ";
        int c;
        cin >> c;
        if(c == 0){
            cout << "Exit program!!!";
            break;
        }
        if(c == 1){
            int numSec, offset, x;
            cout << "Input ordinal of sector: ";
            cin >> numSec;
            cout << "Input offset: ";
            cin >> offset;
            cout << "Input integer: ";
            cin >> x;
            if(numSec < 0){
                cout << "Ordinal can not negative!!!\n";
                continue;
            }
            bool ok = writeInt(dsk, numSec, x, offset);
            if(ok)
                cout << "Write to sector success!\n";
            else
                cout << "ERROR! Can not write to sector!\n";
        }
        if(c == 2){
            int numSec, offset, x;
            cout << "Input ordinal of sector: ";
            cin >> numSec;
            cout << "Input offset: ";
            cin >> offset;
            if(numSec < 0){
                cout << "Ordinal can not negative!!!\n";
                continue;
            }
            auto [ok, res] = readInt(dsk, numSec, offset);
            if(ok)
                cout << "Read success!, Result: " << res << '\n';
            else
                cout << "ERROR! Can not read integer from sector!\n";
        }
        if(c == 3){
            cout << "How many sector want to write: ";
            int ns;
            cin >> ns;
            cout << "Index sector to write: ";
            int numS;
            cin >> numS;
            if(numS < 0){
                cout << "Ordinal can not negative!!!\n";
                continue;
            }
            char* buf = new char[ns * 512];
            cout << "Input text: " ;
            scanf("%s", buf);
			cout << "buf: " << buf << '\n';
            bool ok = WriteSector(dsk, buf, ns, numS);
            if(ok)
                cout << "Write success!\n";
            else
                cout << "ERROR!\n";
        }
        if(c == 4){
            cout << "How many sector want to read?: ";
            int ns;
            cin >> ns;
            cout << "What index sector to read?: ";
            int numS;
            cin >> numS;
            if(numS < 0){
                cout << "Ordinal can not negative!!!\n";
                continue;
            }
            char* buf = new char[ns * 512];
            bool ok = ReadSector(dsk, buf, ns, numS);
            if(ok){
                cout << "Read success!, Your data: \n";
                for(int i = 0; i < 512 * ns; ++i){
                    printf("%c", buf[i]);
                    if(i % 16 == 0)
                        cout << '\n';
                }
            }
            else{
                cout << "ERROR!\n";
            }
        }
    }
    return 0;
}
