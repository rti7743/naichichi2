README
------
1. Download OpenSSL 1.0.1c* from official web site. 
2. Put vsbuild folder (where this file is located) into openssl folder, the folder structure should be looking like:
	\openssl
		\apps
		\bugs
		…
		\VMS
		\vsbuild
		ACKNOWLEDGMENTS
		…
3. Open \openssl\vsbuild\openssl.2010.sln and build it. All generated binaries are located at \openssl\bin\$(Platform)\$(Configuration).

* The project files are created based on 1.0.1c. Very likely they won't work with a later version and will need changes.
