# AUTHOR: TYLER PRICE

# EDUCATIONAL PURPOSES ONLY

# Usage: Specify Process ID (PID) and the DLL Path to Inject

import sys,ctypes
from ctypes import *

kernel32 = windll.kernel32

pid = raw_input("Enter PID: ")

dll_path = raw_input("Enter DLL Path: ")

print "[*] Opening Process....",pid 
hproc = kernel32.OpenProcess(0x1F0FFF,False,int(pid))
print "[*] Allocation memory."
ADDRS = kernel32.VirtualAllocEx(hproc, 0,len(dll_path),0x3000,0x04)
print "[*] Writing into Process.."
kernel32.WriteProcessMemory(hproc,ADDRS,dll_path,len(dll_path),byref(c_int(0)))
print "[*] Getting Module Handle.."
k32dll = kernel32.GetModuleHandleA("kernel32")
print "[*] Get Process Address"
loadlib_func = kernel32.GetProcAddress(k32dll,"LoadLibraryA")
print "[*] Remote Thread.."
if not kernel32.CreateRemoteThread(hproc,None,0,loadlib_func,ADDRS,0,byref(c_ulong(0))):
	print "[*] Injection Failed"
	sys.exit(0)
else:
	print "[*] Injection Successfull"
