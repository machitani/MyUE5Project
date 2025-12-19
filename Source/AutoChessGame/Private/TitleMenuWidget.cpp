#include "TitleMenuWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/WidgetSwitcher.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "Kismet/GameplayStatics.h"
#include "TMAGameInstance.h"

static UButton* MakeButton(UWidgetTree* Tree, const FString& Label)
{
    UButton* Btn = Tree->ConstructWidget<UButton>(UButton::StaticClass());
    UTextBlock* Txt = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Txt->SetText(FText::FromString(Label));
    Btn->AddChild(Txt);
    return Btn;
}
TSharedRef<SWidget> UTitleMenuWidget::RebuildWidget()
{
    UE_LOG(LogTemp, Warning, TEXT("[TitleMenuWidget] RebuildWidget"));

    // ★ すでにWidgetTreeがある場合もあるので、一回クリアするのが安全
    if (WidgetTree)
    {
        WidgetTree->RootWidget = nullptr;
    }

    UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
    WidgetTree->RootWidget = RootCanvas;

    Switcher = WidgetTree->ConstructWidget<UWidgetSwitcher>(UWidgetSwitcher::StaticClass());
    RootCanvas->AddChild(Switcher);

    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Switcher->Slot))
    {
        CanvasSlot->SetAnchors(FAnchors(0, 0, 1, 1));
        CanvasSlot->SetOffsets(FMargin(0));
    }

    // ===== Page0: Title =====
    UVerticalBox* PageTitle = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
    Switcher->AddChild(PageTitle);

    {
        UTextBlock* TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        TitleText->SetText(FText::FromString(TEXT("Toy Mech Arena")));
        if (UVerticalBoxSlot* VS = PageTitle->AddChildToVerticalBox(TitleText))
        {
            VS->SetPadding(FMargin(20));
        }
    }

    StartButton = MakeButton(WidgetTree, TEXT("Start"));
    if (UVerticalBoxSlot* VS = PageTitle->AddChildToVerticalBox(StartButton))
    {
        VS->SetPadding(FMargin(20));
    }

    // ===== Page1: Stage Select =====
    UVerticalBox* PageStage = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
    Switcher->AddChild(PageStage);

    {
        UTextBlock* Header = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        Header->SetText(FText::FromString(TEXT("Select Stage")));
        if (UVerticalBoxSlot* VS = PageStage->AddChildToVerticalBox(Header))
        {
            VS->SetPadding(FMargin(20));
        }
    }

    Stage1Button = MakeButton(WidgetTree, TEXT("Stage 1"));
    Stage2Button = MakeButton(WidgetTree, TEXT("Stage 2"));
    Stage3Button = MakeButton(WidgetTree, TEXT("Stage 3"));
    BackButton = MakeButton(WidgetTree, TEXT("Back"));

    PageStage->AddChildToVerticalBox(Stage1Button);
    PageStage->AddChildToVerticalBox(Stage2Button);
    PageStage->AddChildToVerticalBox(Stage3Button);
    PageStage->AddChildToVerticalBox(BackButton);

    // 初期ページ
    Switcher->SetActiveWidgetIndex(0);

    // ★ ここが重要：最後にSuperでSlateを組ませる
    return Super::RebuildWidget();
}

void UTitleMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();
    UE_LOG(LogTemp, Warning, TEXT("[TitleMenuWidget] NativeConstruct"));

    // RebuildWidgetは複数回走ることがあるので、都度バインド（重複回避したいなら一工夫）
    if (StartButton)  StartButton->OnClicked.AddDynamic(this, &UTitleMenuWidget::OnStartClicked);
    if (BackButton)   BackButton->OnClicked.AddDynamic(this, &UTitleMenuWidget::OnBackClicked);
    if (Stage1Button) Stage1Button->OnClicked.AddDynamic(this, &UTitleMenuWidget::OnStage1Clicked);
    if (Stage2Button) Stage2Button->OnClicked.AddDynamic(this, &UTitleMenuWidget::OnStage2Clicked);
    if (Stage3Button) Stage3Button->OnClicked.AddDynamic(this, &UTitleMenuWidget::OnStage3Clicked);
}

void UTitleMenuWidget::OnStartClicked() { if (Switcher) Switcher->SetActiveWidgetIndex(1); }
void UTitleMenuWidget::OnBackClicked() { if (Switcher) Switcher->SetActiveWidgetIndex(0); }

void UTitleMenuWidget::OnStage1Clicked() { GoToStage(1); }
void UTitleMenuWidget::OnStage2Clicked() { GoToStage(2); }
void UTitleMenuWidget::OnStage3Clicked() { GoToStage(3); }

void UTitleMenuWidget::GoToStage(int32 StageIndex)
{
    if (UTMAGameInstance* GI = Cast<UTMAGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
    {
        GI->SelectedStageIndex = StageIndex;
    }
    UGameplayStatics::OpenLevel(this, FName(TEXT("L_Game")));
}


