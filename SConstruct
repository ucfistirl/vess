
# Python imports
import ConfigParser, platform, os, stat


# VESS Configuration

# Get the platform name and figure out which configuration file to open
opSystem = platform.system()
configFilename = 'config/Configuration.' + opSystem

# Try to open the config file
try:
   configFile = file(configFilename)
except:
   print """

      Unable to open VESS configuration file \""""  + configFilename + """\"
      Please make sure it exists.

"""
   Exit(1)

# Parse the config file
config = ConfigParser.ConfigParser()
config.readfp(configFile)


# Functions

# Borrowed from id, this takes a prefix and adds it to each filename and
# then returns them as a list
def buildList(sPrefix, sString):
   sList = Split(sString)
   for i in range(len(sList)):
      sList[i] = sPrefix + '/' + sList[i]
   return sList


# This takes a base path, a subpath include path, a subpath lib path
# and a string of libs, and then adds the appropriate data to our
# internal variables (include path, lib path and list of libs)
def addExternal(basePath, subIncPath, subLibPath, libs):
   includeDir = Split(basePath + subIncPath)
   libDir = Split(basePath + subLibPath)
   extIncPath.extend(includeDir)
   extLibPath.extend(libDir)
   extLibs.extend(Split(libs))

# Embeds a Visual Studio-style manifest into the given output target
# (only under Windows)
def embedManifest(environment, target, suffix):
   if str(Platform()) == 'win32':
      # The suffix indicates the file type (1=.exe, 2=.dll)
      environment.AddPostAction(target,
                                'mt.exe -nologo -manifest ${TARGET}.manifest \
                                -outputresource:$TARGET;' + str(suffix))


# Set the initial CFLAGS, defines and include path
if opSystem == 'Windows':

   # Flags for the VC++ compiler
   # /nologo      = Don't print the compiler banner
   # /MD          = Use multithreaded DLL runtime
   # /O2          = Optimize for speed
   # /EHsc        = Exception handling
   # /W3          = Warning level
   # /Zc:forScope = Use standard C++ scoping rules in for loops
   # /GR          = Enable C++ run-time type information
   # /Gd          = Use __cdecl calling convention
   # /Z7          = Generate debug information
   compileFlags = Split('/nologo /MD /O2 /EHsc /W3 /Zc:forScope /GR /Gd /Z7')

   # Additional flags to disable useless warnings
   compileFlags += Split('/wd4091 /wd4275 /wd4290')

   # Import ATLAS symbols and export VESS symbols
   defines = Split('ATLAS_SYM=IMPORT VESS_SYM=EXPORT')

   # Disable deprecation warnings for "insecure" and "nonstandard" functions
   defines += Split('_CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_DEPRECATE')

   # Flags for the VC++ linker
   # /DEBUG          = Generate debugging information
   # /OPT:REF        = Optimize away unreferenced code
   # /OPT:ICF        = Optimize away redundant function packages
   # /INCREMENTAL:NO = Do not perform incremental linking
   linkFlags = Split('/DEBUG /OPT:REF /OPT:ICF /INCREMENTAL:NO')

else:

   # Flags for gcc (generate debug information and optimize)
   compileFlags = Split('-g -O')

   # Import ATLAS symbols and export VESS symbols
   defines = Split('ATLAS_SYM=IMPORT VESS_SYM=EXPORT')

   # No linker flags needed
   linkFlags = []


# Set up external library lists
extIncPath = []
extLibPath = []
extLibs = []

# Create another set of lists for ATLAS
atlasPath = config.get('base', 'atlasPath')
atlasIncPath = buildList(atlasPath, 
                         'communication container foundation math os util xml')
atlasLibPath = Split(atlasPath)
atlasLibs = Split('atlas')


# Depending on platform, add the external libraries that ATLAS requires
# (Windows requires more to be linked in than Linux does)
if opSystem == 'Windows':

   # Add the RTI
   rtiPath = config.get('base', 'rtiPath')
   addExternal(rtiPath, '/include/1.3', '/lib/winnt_vc++-9.0', 'librti13')

   # Add libxml2
   xmlPath = config.get('base', 'xmlPath')
   addExternal(xmlPath, '/include', '/lib', 'libxml2')

   # Add iconv
   iconvPath = config.get('base', 'iconvPath')
   addExternal(iconvPath, '/include', '/lib', 'iconv')

   # Add pthreads
   pthreadPath = config.get('base', 'pthreadPath')
   addExternal(pthreadPath, '/include', '/lib', 'pthreadVC2')

   # Add the msinttypes headers
   msinttypesPath = config.get('base', 'msinttypesPath')
   extIncPath.extend(Split(msinttypesPath + '/include'))

   # Add the OpenGL extension headers
   glPath = config.get('base', 'glPath')
   extIncPath.extend(Split(glPath + '/include'))

   # Add the Windows-specific libraries (already in main path)
   extLibs.extend(Split('ws2_32 winmm opengl32 user32 gdi32'))

elif opSystem == 'Linux':

   # Add the RTI
   rtiPath = config.get('base', 'rtiPath')
   addExternal(rtiPath, '/include/1.3', '/lib/linux_g++-4.4', 'rti13')

   # Add libxml2
   xmlPath = config.get('base', 'xmlPath')
   addExternal(xmlPath, '/include/libxml2', '/lib', 'xml2')

   # Add pthreads
   pthreadPath = config.get('base', 'pthreadPath')
   addExternal(pthreadPath, '/include', '/lib', 'pthread')

   # Add the X libraries (already in main path)
   extLibs.extend(Split('Xi'))

else:

   print 'Platform is: ' + platform
   print 'VESS is not supported on this platform.'
   Exit(1)


# Combine the various path and library lists
# include path
incPath = []
incPath.extend(atlasIncPath)
incPath.extend(extIncPath)

libPath = []
libPath.extend(atlasLibPath)
libPath.extend(extLibPath)

libs = []
libs.extend(atlasLibs)
libs.extend(extLibs)


# Create an environment object to store the build environment settings
vessEnv = Environment()
vessEnv.Append(CCFLAGS = compileFlags)
vessEnv.Append(CPPDEFINES = defines)
vessEnv.Append(CPPPATH = incPath)
vessEnv.Append(LIBPATH = libPath)
vessEnv.Append(LIBS = libs)
vessEnv.Append(LINKFLAGS = linkFlags)


# Create the list of subdirectories
vessSubdirs = []
vessSubdirs.extend(Split('util llio graphics io sound scent motion avatar'))
vessSubdirs.extend(Split('environment system menu'))

# Compile the set of VESS objects
vessObjs = []
for dir in vessSubdirs:

   # Set up the next SConscript file to be run
   script = dir + '/SConscript'

   # Run the script, passing it the current environment, the configuration
   # file parser, and the extra functions we need.  The script returns a
   # tuple containing the list of objects and the new build environment,
   # extended with any additional headers and libraries we need.
   buildTuple = SConscript(script, 'vessEnv config buildList addExternal')

   # Extend the list of objects
   vessObjs.extend(buildTuple[0])

   # Grab the new environment
   vessEnv = buildTuple[1]

# Finally, compile the VESS shared library
vess = vessEnv.SharedLibrary('vess', vessObjs)

# Only compile the "vess" target by default
Default(vess)

# Under Windows, embed the manifest into the .dll
embedManifest(vessEnv, vess, 2)

# Set up a test target so we can build test programs with the current VESS
# environment, first see if "test" exists
if os.path.exists('test'):

   # Make sure "test" is a directory
   testMode = os.stat('test').st_mode
   if stat.S_ISDIR(testMode):
      SConscript('test/SConscript', 'vessEnv')

