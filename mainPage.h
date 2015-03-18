/*******************************************************************************
 * @mainpage
 * 
 * @section faq Frequently Asked Questions
 * 
 * <ol type="I">
 *	<li>What's all this about?</li>
 *	<ul type="none">
 * 		<li>DeadStrip is a tool designed to perform dependency analysis and dead
 *		function/data removal on MinGW/gcc generated object files.</li>
 *	</ul>
 *		
 *	<li>But doesn't the compiler do this already?</li>
 *	<ul type="none">
 *		<li>No. The compiler doesn't know enough about the global contexts an
 *		object file might be linked into to do a reasonable strip of unreferenced
 *		code and data. It does however perform local optimization, i.e. in
 *		each object file.</li>
 *	</ul>
 *	
 *	<li>Okay, what about the linker?</li>
 *	<ul type="none">
 *		<li>Yes, the linker would be able to do dependency analysis, because it's
 *		got a global view onto the data. There's even a command line switch that
 *		enables this sort of global optimization; unfortunately it doesn't work
 *		under Windows.</li>
 *	</ul>
 *	
 *	<li>What is 'dead code stripping' anyway?</li>
 *	<ul type="none">
 *		<li>Removing code from your executable file that can be proven not to be
 *		in use. This reduces the total size of your apps.</li>
 *	</ul>
 *	
 *	<li>Couldn't I just remove all the unused functions by commenting them out?
 *	</li>
 *	<ul type="none">
 *		<li>You could, of course. But you're likely to forget some dependencies on
 *		larger projects, resulting in loads of linker errors and many hours of
 *		fun. Just imagine a larger framework you'd like to include in your
 *		project. You use just one function, because you're too lazy to reinvent
 *		the wheel. The compiler won't complain and the linker will link a nice XXL
 *		executable, not only including the PNG-loader but also a whole database
 *		management system, to manage your images in C++. This is the gap DeadStrip
 *		aims to fill.</li>
 *	</ul>
 *	
 *	<li>Then DeadStrip is an alternative linker?</li>
 *	<ul type="none">
 *		<li>No. You still use whatever linker you like to use. DeadStrip only
 *		hooks into the compiler-linker toolchain and analyses the object files
 *		before the linker sees them, removing all unused sections. When it's done,
 *		it calls the linker.</li>
 *	</ul>
 *	
 *	<li>Aren't there any other tools, that do the same already?</li>
 *	<ul type="none">
 *		<li>I've searched about one month for some tool like this, reading nearly
 *		every newsgroup entry containing the word GCC (there's a lot of them,
 *		trust me) and all I found, was some alternative linker contained in a
 *		large compiler-IDE framework that said it could do what I wanted. The
 *		final executable was larger than before when I was still linking using ld.
 *		Additionally ld has much more options when it comes down to fine tuning.
 *		So I decided to write that tool myself and still be able to use my
 *		favourite linker.</li>
 *	</ul>
 *	
 *	<li>I'm missing feature XY.</li>
 *	<ul type="none">
 *		<li>Then you've got a couple of options. If this feature is of major
 *		importance to you and you're not able to write it for yourself, ask me. If
 *		I like the idea, I might be implementing it and provide an update. \n
 *		But more reliable than me, is you. If you're able to write it for
 *		yourself, either by attaching another tool to the output of DeadStrip, or
 *		by contributing to the tools code, feel free to do so. DeadStrip provides
 *		XML-like output of various computed information and comes with full
 *		platform independent C source code (see below). I would be happy to hear 
 *		from you in those cases. Although I strictly encourage you to contribute
 *		to the source, please don't provide links on your site to your modified
 *		version saying that this is the "official" one (that is, my one). You can,
 *		however, modify your own version and present it as a modification from
 *		you. Credits are not necessary.</li>
 *	</ul>
 *	
 *	<li>I found a bug.</li>
 *	<ul type="none">
 *		<li>Mail me with a detailed description of the error and provide some
 *		source code, if necessary, or try to fix it yourself and mail me
 *		afterwards :). My e-mail adress is weber &auml;t informatik.hu-berlin.de.
 *		</li>
 *	</ul>
 *	
 * </ol>
 * 
 * @section syntax Syntax
 * 
 * @verbatim
 deadstrip [Options] file...
   Options:
     --help                display this help
     --dcmd                dumps the command line
     --ddis                dumps discarted sections
     --duse                dumps used sections
     --dmap                dump the dependency map
     --linker <filename>   use alternative linker (default: ld)
     --dnrm                do not remove any sections
     --save <item>         save an item and its dependencies
       > just pass the decorated variable/function name, not the section
       > the main function gets saved by default
 @endverbatim
 * 
 * @section notes Additional Notes
 * 
 * <ul>
 *	<li>all dumps are printed to stdout - use the pipe symbol to redirect them,
 *	if necessary</li>
 *	<li>all switches that DeadStrip doesn't recognize are passed to the linker
 *	</li>
 *	<li>you should use the \c -ffunction-sections and/or \c -fdata-sections
 *	options, in order to generate appropriate section-partitioning; if you
 *	don't, DeadStrip won't be able to strip anything</li>
 *	<li>excessive section partitioning will increase the size of your exe, but
 *	compression tools like <a href="http://upx.sourceforge.net">UPX</a> can
 *	merge all sections afterwards</li>
 *	<li>when saving sections, be sure to only provide the decorated
 *	function/data	name, not the prefix that the compiler generates</li>
 *	<ul>
 *		<li>plain C functions are not decorated at all, so when you need to save a
 *		function called "mainCRTStartup", pass this name to DeadStrip</li>
 *		<li>C++ functions are decorated according to their parameters, namespace
 *		and member information; you may need to look up their names using objdump
 *		from the binutils</li>
 *		<li>you can optionally place the functions that you want to be save into a
 *		section of your choice by using gcc's
 *		<tt>__attribute__((section("myName")))</tt>. DeadStrip doesn't strip code
 *		located in unknown sections.</li>
 *	</ul>
 *	<li>DeadStrip depends on some tools, that are provided by binutils</li>
 *	<ul>
 *		<li>namely objdump and objcopy</li>
 *		<li>that means, you should have DeadStrip located, where those two tools
 *		reside (e.g. MinGW/bin/)</li>
 *	</ul>
 * </ul>
 * 
 * @section install Installation
 * 
 * Just drop the executable into the directory of your binutils, then configure
 * your IDE or makefile to call DeadStrip instead of the linker, but provide the
 * original linker using the --linker-switch. Finally enable
 * \c -ffunction-sections and/or \c -fdata-sections in your compiler for your
 * release builds.
 * 
 * @section downloads Downloads
 * @subsection examples Example output
 * 
 * \li <a href="outCmdLn.txt">Command line</a>
 * \li <a href="outDpnMp.txt">Dependency map</a>
 * \li <a href="outUSect.txt">Used sections</a>
 * \li <a href="outUnSect.txt">Unused sections</a>
 * 
 * @subsection program Program related
 * 
 * \li <a href="DeadStrip_exe.zip">zipped final executable</a>
 * \li <a href="DeadStrip_src.zip">zipped source code</a>
 * 
 * @section history Version History
 * @subsection v11 Version 1.1
 * 
 * \li fixed a severe bug concerning the stripping of virtual functions.
 * DeadStrip should be completely compatible with C++ now
 * \li noticed that references from the relocation section could safely be
 * ignored during analysis. Code referenced only from there gets stripped now.
 * 
 * @subsection v10 Version 1.0
 * 
 * \li This is the first release version, everything seems to work.
 * 
 ******************************************************************************/
