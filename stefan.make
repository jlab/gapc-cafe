DIR_COMPILE=/homes/sjanssen/Desktop/BIBI/gapc-pages/Temp/
#~ DIR_INSTALL=/vol/gapc/share/www/
DIR_INSTALL=$(DIR_COMPILE)

BOOST_SUFFIX=-gcc45-mt-1_52
BOOST_PREFIX=/vol/gapc
BOOST_INCLUDE=$(BOOST_PREFIX)/include/boost-1_52

GAPC_PREFIX=/vol/gapc
LDFLAGS_EXTRA=-R/vol/gcc-4.5/lib/amd64/
WT_PREFIX=/vol/gapc
TMPDIR=.
MAKE=/vol/gnu/bin/make

build:
	umask 022;
	if [ ! -d "processxx/" ]; then \
		hg clone https://bitbucket.org/gsauthof/processxx; \
	else \
		cd processxx && hg revert --all; \
	fi;
	if [ ! -f "gap.wt" ]; then \
		sed -i "s|/usr/bin/make|$(MAKE)|" form.cc; \
		sed -i "s|-lboost_thread\$$(BOOST_SUFFIX)|-lboost_thread\$$(BOOST_SUFFIX) -lboost_system\$$(BOOST_SUFFIX)|" processxx/Makefile; \
		make -C processxx CXXFLAGS="-g -m64 -Wall -pthreads -I$(BOOST_INCLUDE)/" LDFLAGS="-L$(BOOST_PREFIX)/lib -m64" BOOST_SUFFIX="$(BOOST_SUFFIX)" libprocess.a; \
		sed -i "s|find_package(Boost 1.38 COMPONENTS system REQUIRED)|set(BOOST_ROOT $(BOOST_PREFIX))\nset(Boost_USE_MULTITHREADED = FALSE)\nfind_package(Boost 1.52 COMPONENTS system REQUIRED)|" CMakeLists.txt; \
		sed -i "s|SET_PROPERTY(TARGET process PROPERTY IMPORTED_LOCATION \$$ENV{HOME}/program/process/libprocess.a)|SET_PROPERTY(TARGET process PROPERTY IMPORTED_LOCATION ./processxx/libprocess.a)|" CMakeLists.txt; \
		sed -i "s|LINK_DIRECTORIES(/mnt/gms/local/wt-3.1.9/lib)|LINK_DIRECTORIES($(GAPC_PREFIX)/lib)|" CMakeLists.txt; \
		sed -i "s|INCLUDE_DIRECTORIES(/mnt/gms/local/wt-3.1.9/include \$$ENV{HOME}/program/process)|INCLUDE_DIRECTORIES($(GAPC_PREFIX)/include/ $(BOOST_INCLUDE)/ ./processxx)|" CMakeLists.txt; \
		cmake CMakeLists.txt \
			-DCMAKE_EXE_LINKER_FLAGS="-lwt -lrt -lboost_thread$(BOOST_SUFFIX) -lboost_filesystem$(BOOST_SUFFIX) -lboost_signals$(BOOST_SUFFIX) -lboost_system$(BOOST_SUFFIX) $(LDFLAGS_EXTRA)" \
			-DCMAKE_MODULE_LINKER_FLAGS="-lwt -lrt -lboost_thread$(BOOST_SUFFIX) -lboost_filesystem$(BOOST_SUFFIX) -lboost_signals$(BOOST_SUFFIX) -lboost_system$(BOOST_SUFFIX) $(LDFLAGS_EXTRA)" \
			-DCMAKE_SHARED_LINKER_FLAGS="-lwt -lrt -lboost_thread$(BOOST_SUFFIX) -lboost_filesystem$(BOOST_SUFFIX) -lboost_signals$(BOOST_SUFFIX) -lboost_system$(BOOST_SUFFIX) $(LDFLAGS_EXTRA)" \
			-DCMAKE_CXX_FLAGS="-pthreads -m64" \
			-DBOOST_INCLUDEDIR="$(BOOST_INCLUDE)/"; \
		$(MAKE) -f Makefile; \
	fi;
	mkdir -p $(DIR_COMPILE);
	mkdir -p $(DIR_COMPILE)/cached;
	mkdir -p $(DIR_COMPILE)/output;
	mkdir -p $(DIR_COMPILE)/log;
	mkdir -p $(DIR_COMPILE)/grammar;
	mkdir -p $(DIR_COMPILE)/app;
	mkdir -p $(DIR_COMPILE)/pages;
	cp -r $(WT_PREFIX)/share/Wt/resources $(DIR_COMPILE)/app/
	cd $(DIR_COMPILE) && ln -sf app/resources
	cp -u gap.wt $(DIR_COMPILE)
	cp -u menu.rc $(DIR_COMPILE)
	cp -u grammar/* $(DIR_COMPILE)/grammar
	cp -u pages/menu.html_ $(DIR_COMPILE)/pages
	cd web && flex repl.l && g++ -m64 lex.repl_.c -o repl -I$(BOOST_INCLUDE) $(LDFLAGS_EXTRA)
	export TMPDIR
	$(MAKE) -f web/web.mf SRC=pages DST=$(DIR_COMPILE) INSERT=web/repl
	cp -u pages/*_bib_bib.html $(DIR_COMPILE)
	cp -u resources/gap.css $(DIR_COMPILE)/resources
	cp -u resources/favicon.ico $(DIR_COMPILE)
	cp -u resources/header_noTitle.png $(DIR_COMPILE)/resources

run:
	$(DIR_COMPILE)/gap.wt --docroot='.;$(DIR_COMPILE)/app/resources,$(DIR_COMPILE)/resources,$(DIR_COMPILE)/grammar' \
  --deploy-path '/app/' \
  --http-address 127.0.0.1 \
  --http-port 25777 \
  --accesslog $(DIR_COMPILE)/gap.log \
  > $(DIR_COMPILE)/x.log 2>&1
	
distclean:
	rm -rf $(DIR_COMPILE);
	rm -rf CMakeFiles
	rm -f CMakeCache.txt
	hg revert CMakeLists.txt
	rm -f cmake_install.cmake
	rm -f Makefile
	rm -f hl.cc
	rm -f gap.wt
	rm -f web/lex.repl_.c
	rm -f web/repl
	rm -f pages/adp_bib.html
	rm -f pages/adp_bib_bib.html
	rm -f pages/bio_bib.html
	rm -f pages/bio_bib_bib.html
	rm -f pages/gap_bib.html
	rm -f pages/gap_bib_bib.html
