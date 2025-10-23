// Fill out your copyright notice in the Description page of Project Settings.


#include "AAutoChessGameMode.h"

AAAutoChessGameMode::AAAutoChessGameMode()
{
	PlayerControllerClass = ACustomPlayerController::StaticClass();

	//DefaultPawnClass = nullptr;
}