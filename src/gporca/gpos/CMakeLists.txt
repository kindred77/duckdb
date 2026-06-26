include_directories(src/include/duckdb/optimizer/gporca)

add_library_unity(
        duckdb_gpos
        OBJECT
        common/CAutoTimer.cpp
        common/CBitSet.cpp
        common/CBitSetIter.cpp
        common/CBitVector.cpp
        common/CDebugCounter.cpp
        common/CHeapObject.cpp
        common/clibwrapper.cpp
        common/CMainArgs.cpp
        common/CRandom.cpp
        common/CRefCount.cpp
        common/CStackDescriptor.cpp
        common/CStackObject.cpp
        common/CTimerUser.cpp
        common/CWallClock.cpp
        common/syslibwrapper.cpp

        error/CAutoExceptionStack.cpp
        error/CAutoTrace.cpp
        error/CErrorContext.cpp
        error/CErrorHandlerStandard.cpp
        error/CException.cpp
        error/CFSimulator.cpp
        error/CLogger.cpp
        error/CLoggerStream.cpp
        error/CLoggerSyslog.cpp
        error/CMessage.cpp
        error/CMessageRepository.cpp
        error/CMessageTable.cpp
        error/CMiniDumper.cpp
        error/CSerializable.cpp
        error/ILogger.cpp

        io/CFileDescriptor.cpp
        io/CFileReader.cpp
        io/CFileWriter.cpp
        io/COstream.cpp
        io/COstreamBasic.cpp
        io/COstreamFile.cpp
        io/COstreamStdString.cpp
        io/COstreamString.cpp
        io/ioutils.cpp

        memory/CAutoMemoryPool.cpp
        memory/CCacheFactory.cpp
        memory/CMemoryPool.cpp
        memory/CMemoryPoolManager.cpp
        memory/CMemoryPoolTracker.cpp
        memory/CMemoryVisitorPrint.cpp

        string/CStringStatic.cpp
        string/CWString.cpp
        string/CWStringBase.cpp
        string/CWStringConst.cpp
        string/CWStringDynamic.cpp
        string/CWStringStatic.cpp

        task/CAutoSuspendAbort.cpp
        task/CAutoTaskProxy.cpp
        task/CAutoTraceFlag.cpp
        task/CTask.cpp
        task/CTaskContext.cpp
        task/CTaskLocalStorage.cpp
        task/CTaskSchedulerFifo.cpp
        task/CWorker.cpp
        task/CWorkerPoolManager.cpp
        task/ITask.cpp
        task/IWorker.cpp

        _api.cpp
        dlfcn.c
        utils.cpp
        )
set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:duckdb_gpos>
        PARENT_SCOPE)
