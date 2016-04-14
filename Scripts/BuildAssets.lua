--[[
	This file contains the logic for building assets
]]

-- Static Data Initialization
--===========================

local s_AuthoredAssetDir, s_BuiltAssetDir, s_BinDir
do
	-- AuthoredAssetDir
	do
		print("About to initialize Key")
		local key = "AuthoredAssetDir"
		print(key)
		local errorMessage
		s_AuthoredAssetDir, errorMessage = GetLuaEnvironmentVariable( key )
		if not s_AuthoredAssetDir then
			error( errorMessage )
		end
	end
	-- BuiltAssetDir
	do
		local key = "BuiltAssetDir"
		local errorMessage
		s_BuiltAssetDir, errorMessage = GetLuaEnvironmentVariable( key )
		if not s_BuiltAssetDir then
			error( errorMessage )
		end
	end

	-- BinDir
	do
		local key = "BinDir"
		local errorMessage
		s_BinDir, errorMessage = GetLuaEnvironmentVariable( key )
		if not s_BinDir then
			error( errorMessage )
		end
	end
end

-- Function Definitions
--=====================

local function BuildAsset( i_relativePath, i_builderFileName, optional, dependency)
	-- Get the absolute paths to the source and target
	print("Checking the optional parameter")
	print(optional)

	local path_source = s_AuthoredAssetDir .. i_relativePath
	local path_target = s_BuiltAssetDir .. i_relativePath
	-- If the source file doesn't exist then it can't be built
	do
		local doesSourceExist = DoesFileExist( path_source )
		if not doesSourceExist then
			OutputErrorMessage( "The source asset doesn't exist", path_source )
			return false
		end
	end

	-- Find the appropriate builder for this asset type
	local path_builder
	do
		print("Printing path builder 1 ")

		if type( i_builderFileName ) == "string" then
			print(path_builder)
			path_builder = s_BinDir .. i_builderFileName
			if not DoesFileExist( path_builder ) then
				error( "The specified builder (\"" .. path_builder .. "\") doesn't exist", stackLevel )
			end
		else
			error( "The specified builder file name must be a string (instead of a " ..
				type( i_builderFileName ) .. ")", stackLevel )
		end
	end
	
	
		-- Find the appropriate builder for this asset type
	local dependency
	do
		print("Printing dependency 1 ")
		if dependency ~= nil then
			if type( dependency ) == "string" then
				print(dependency)
				dependency = s_BinDir .. dependency
				if not DoesFileExist( dependency ) then
					error( "The specified dependency (\"" .. dependency .. "\") doesn't exist", stackLevel )
				end
			else
				error( "The specified dependency file name must be a string (instead of a " ..
					type( dependency ) .. ")", stackLevel )
			end
		end
	end

	-- Decide if the target needs to be built
	local shouldTargetBeBuilt
	do
			print("Printing should target builder 1 ")

		-- The simplest reason a target should be built is if it doesn't exist
		local doesTargetExist = DoesFileExist( path_target )
		if doesTargetExist then
			-- Even if the target exists it may be out-of-date.
			-- If the source has been modified more recently than the target
			-- then the target should be re-built.
			local lastWriteTime_source = GetLastWriteTime( path_source )
			local lastWriteTime_target = GetLastWriteTime( path_target )
			shouldTargetBeBuilt = lastWriteTime_source > lastWriteTime_target
			if not shouldTargetBeBuilt then
				-- Even if the target was built from the current source
				-- the builder may have changed which could cause different output
				local lastWriteTime_builder = GetLastWriteTime( path_builder )
				shouldTargetBeBuilt = lastWriteTime_builder > lastWriteTime_target
			end
		else
			shouldTargetBeBuilt = true;
		end
	end
	
	-- Decide if the dependency has changed to build shaders
	-- Build the target if necessary
	if dependency~=nil then
		local shouldShadersBeBuilt
		do
				print("Printing should target builder 1 ")

			-- The simplest reason a target should be built is if it doesn't exist
			local doesTargetExist = DoesFileExist( path_target )
			if doesTargetExist then
				-- Even if the target exists it may be out-of-date.
				-- If the source has been modified more recently than the target
				-- then the target should be re-built.
				local lastWriteTime_source = GetLastWriteTime( path_source )
				local lastWriteTime_target = GetLastWriteTime( path_target )
				shouldShadersBeBuilt = lastWriteTime_source > lastWriteTime_target
				if not shouldShadersBeBuilt then
					-- Even if the target was built from the current source
					-- the builder may have changed which could cause different output
					local lastWriteTime_builder = GetLastWriteTime( dependency )
					shouldShadersBeBuilt = lastWriteTime_builder > lastWriteTime_target
				end
			else
				shouldShadersBeBuilt = true;
			end
		end
	end
	


	if shouldTargetBeBuilt or shouldShadersBeBuilt then
		-- Create the target directory if necessary
		CreateDirectoryIfNecessary( path_target )

		
		-- Build
		do
					print("---------------- INSIDE BUILD---------  ")

			-- The command starts with the builder
			local command = "\"" .. path_builder .. "\""
			-- The source and target path must always be passed in
			local arguments = "\"" .. path_source .. "\" \"" .. path_target .. "\""
			-- If you create a mechanism so that some asset types could include extra arguments
			-- you would concatenate them here, something like:
			print(arguments)
			print("Checking optional Table")
			print(optional)
			arguments = arguments .. " " .. optional
			--if optional~=nil then arguments = arguments .. " " .. table.concat( optional, " " )
			--end
			-- IMPORTANT NOTE:
			-- If you need to debug a builder you can put print statements here to
			-- find out what the exact command line should be.
			-- "command" should go in Debugging->Command
			-- "arguments" should go in Debugging->Command Arguments
			print("Command ::::::::")
			--print(command)
			--print("Arguments ::::::::")
			print(arguments)
			-- Surround the entire command line in quotes
			local commandLine = "\"" .. command .. " " .. arguments .. "\""
			print(commandLine)
			local result, terminationType, exitCode = os.execute( commandLine )
			if result then
				-- Display a message for each asset
				print("RESULT TRUE")
				print( "Built " .. path_source )
				-- Return the exit code for informational purposes since we have it
				return true, exitCode
			else
				-- The builder should already output a descriptive error message if there was an error
				-- (remember that you write the builder code,
				-- and so if the build process failed it means that _your_ code has returned an error code)
				-- but it can be helpful to still return an additional vague error message here
				-- in case there is a bug in the specific builder that doesn't output an error message
				print("RESULT FALSE")

				do
					local errorMessage = "The command " .. tostring( commandLine )
					if terminationType ~= "signal" then
						errorMessage = errorMessage .. " exited with code "
					else
						errorMessage = errorMessage .. " was terminated by the signal "
					end
					errorMessage = errorMessage .. tostring( exitCode )
					print("This is the error message ::::" .. errorMessage)
					OutputErrorMessage( errorMessage, path_source )
				end
				-- There's a chance that the builder already created the target file,
				-- in which case it will have a new time stamp and wouldn't get built again
				-- even though the process failed
				if DoesFileExist( path_target ) then
					local result, errorMessage = os.remove( path_target )
					if not result then
						OutputErrorMessage( "Failed to delete the incorrectly-built target: " .. errorMessage, path_target )
					end
				end

				return false, exitCode
			end
		end
	else
		return true
	end
end

local function callBuildAsset(assetName, builderFileName,optional,dependency)
		print("Inside call Build Asset")
		print("Asset Name" .. assetName)
				if not BuildAsset( assetName , builderFileName,optional, dependency) then
				wereThereErrors = true
				end
end

local function BuildAssets( i_assetsToBuild )
	local wereThereErrors = false
	print("ASSETS TO BUILD")
	print(i_assetsToBuild)
	for i, assetInfo in ipairs( i_assetsToBuild ) do
		local builderFileName = assetInfo.builder
		print("Builder Name ::")
		print(builderFileName)
		local assets = assetInfo.assets
		local optional = assetInfo.shaderType
		local dependency = assetInfo.dependency
		for j, assetInfo in pairs( assets ) do
			print("Inside the 3rd for loop")
			print(j)
			  if assetInfo["source"]~=nil then assetName = assetInfo["source"] print("Name ::"..assetName) 
			 				callBuildAsset(assetName,builderFileName,optional,dependency)
			  end
			  if assetInfo["target"]~=nil then assetName = assetInfo["target"] print("Name2::"..assetName) 
			 				callBuildAsset(assetName,builderFileName,optional,dependency)
			  end
			  --[[if assetInfo[j]~=nil then assetName = assetInfo[j] print("Name3 ::"..assetName) 
					if(optional~=nil) then
						callBuildAsset(assetName,builderFileName,optional[1],dependency)
					else
					    callBuildAsset(assetName,builderFileName,"")
					end
			  end
			  --]]
			  local i = j
			  while assetInfo[i]~= nil do
			  assetName = assetInfo[i]
				if(optional~=nil) then
							callBuildAsset(assetName,builderFileName,optional[i],dependency)
						else
							callBuildAsset(assetName,builderFileName,"")
						end	
						i = i+ 1

			  end
			  --[[
			  if assetInfo[2]~=nil then assetName = assetInfo[2] print("Name4 ::"..assetName) 
					if(optional~=nil) then
						callBuildAsset(assetName,builderFileName,optional[2],dependency)
					else
						callBuildAsset(assetName,builderFileName,"")
					end

			  end
			    if assetInfo[3]~=nil then assetName = assetInfo[3] print("Name5 ::"..assetName) 
					if(optional~=nil) then
						callBuildAsset(assetName,builderFileName,optional[3],dependency)
					else
						callBuildAsset(assetName,builderFileName,"")
					end

			  end
			  ]]--
		end
	end

	return not wereThereErrors
end

-- Entry Point
--============

-- Command line arguments are represented in Lua as three dots ("...")
local commandLineArgument = ...
print("Reached entry point")
print(commandLineArgument)
if commandLineArgument then
	print("Inside if")
	local path_assetsToBuild = commandLineArgument
	if DoesFileExist( path_assetsToBuild ) then
		print(" file exists")
		local assetsToBuild = dofile( path_assetsToBuild )
		return BuildAssets( assetsToBuild )
	else
		print("No file exists")
		OutputErrorMessage( "The path to the list of assets to build that was provided to BuildAssets.lua as argument #1 (\"" ..
			path_assetsToBuild .. "\") doesn't exist" )
	end
else
	OutputErrorMessage( "BuildAssets.lua must be called with the path to the list of assets to build as an argument" )
end
