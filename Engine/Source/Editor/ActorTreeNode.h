#pragma once

#include "World.h"
#include "Engine/GameFrameWork/Actor.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "Static/EditorManager.h"
#include "ImGui/imgui.h"

class ActorTreeNode
{
    bool                    bVisibility;
    bool                    bUnsaved;
    bool                    bPinned;
    FString                 ItemLabel;
    FString                 Type;
    ActorTreeNode*          Parent;
    TArray<ActorTreeNode*>  Children;
	uint16				    IndexInParent = 0;
    uint32                  UUID;
    AActor*                 Actor;
    bool                    bIsSelected;

public:
	ActorTreeNode(FString InItemLabel, FString InType, ActorTreeNode* InParent, uint32 UUID, AActor* Actor)
		: bVisibility(true)
		, bUnsaved(false)
		, bPinned(false)
		, ItemLabel(std::move(InItemLabel))
		, Type(std::move(InType))
		, Parent(InParent)
		, IndexInParent(InParent ? static_cast<uint16>(Parent->Children.Len()) : 0)
		, UUID(UUID)
		, Actor(Actor)
	{
        if (Parent)
	        Parent->AddChild(this);
        Actor->GetComponents();
	}

	void SetVisibility(bool bInVisibility)
	{
		bVisibility = bInVisibility;
        if (Actor)
        {
            for (UActorComponent* Component : Actor->GetComponents())
            {
                if (UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component))
                {
                    PrimitiveComponent->SetCanBeRendered(bVisibility);
                }
            }
        }
	}
	bool GetVisibility() const { return bVisibility; }

	void SetItemLabel(const FString& InItemLabel) { ItemLabel = InItemLabel; }
    FString GetItemLabel() const { return ItemLabel; }
	void SetType(const FString& InType) { Type = InType; }
    FString GetType() const { return Type; }

	void SetParent(ActorTreeNode* InParent) { Parent = InParent; }
	ActorTreeNode* GetParent() const { return Parent; }

	void AddChild(ActorTreeNode* Child) { Children.Add(Child); }
	void RemoveChild(ActorTreeNode* Child) { Children.Remove(Child); }
	TArray<ActorTreeNode*>& GetChildren() { return Children; }

	void SetUUID(uint32 InUUID) { UUID = InUUID; }
	uint32 GetUUID() const { return UUID; }

	void SetActor(AActor* InActor) { Actor = InActor; }
	AActor* GetActor() const { return Actor; }

    void SetIsSelected(bool bInIsSelected) { bIsSelected = bInIsSelected; }
    bool GetIsSelected() const { return bIsSelected; }

    static void DisplayNode(ActorTreeNode* node, ImGuiSelectionBasicStorage* selection)
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(3);
        const bool is_folder = (!node->Children.IsEmpty());

        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        node_flags |= ImGuiTreeNodeFlags_NavLeftJumpsBackHere; // Enable pressing left to jump to parent
		if (!node->GetParent())
            node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

        node->bIsSelected = selection->Contains(node->GetUUID());
        if (node->bIsSelected)
        {
            if (ImGui::IsWindowFocused())
				FEditorManager::Get().SelectActor(node->Actor);
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }
        else
        {
            node_flags &= ~ImGuiTreeNodeFlags_Selected;
        }

        // Using SetNextItemStorageID() to specify storage id, so we can easily peek into
		// the storage holding open/close stage, using our TreeNodeGetOpen/TreeNodeSetOpen() functions.
        ImGui::SetNextItemSelectionUserData((ImGuiSelectionUserData)(intptr_t)node);
        ImGui::SetNextItemStorageID(node->GetUUID());

        if (is_folder)
        {
            bool open = ImGui::TreeNodeEx(*node->ItemLabel, node_flags);
            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(node);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::Checkbox("##bVisible", &node->bVisibility);
            ImGui::PopStyleVar();
            ImGui::PopID();
            ImGui::TableSetColumnIndex(4);
            ImGui::TextUnformatted(*node->Type);
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(*FString::FromInt(node->GetUUID()));
        	ImGui::TableNextColumn();
            if (node->Parent)
				ImGui::TextUnformatted(*FString::FromInt(node->Parent->GetUUID()));

            if (open)
            {
				for (auto child : node->Children)
					DisplayNode(child, selection);
                ImGui::TreePop();
            }
            else if (ImGui::IsItemToggledOpen())
            {
                TreeCloseAndUnselectChildNodes(node, selection);
            }
        }
		else // Display leaf node
        {
            ImGui::TreeNodeEx(*node->ItemLabel, node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(node);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::Checkbox("##bVisible", &node->bVisibility);
            ImGui::PopStyleVar();
            ImGui::PopID();
            ImGui::TableSetColumnIndex(4);
            ImGui::TextUnformatted(*node->Type);
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(*FString::FromInt(node->GetUUID()));
            ImGui::TableNextColumn();
            if (node->Parent)
				ImGui::TextUnformatted(*FString::FromInt(node->Parent->GetUUID()));
        }
    }

    static bool TreeNodeGetOpen(ActorTreeNode* node)
    {
        return ImGui::GetStateStorage()->GetBool(node->GetUUID());
    }

    static void TreeNodeSetOpen(ActorTreeNode* node, bool open)
    {
        ImGui::GetStateStorage()->SetBool(node->GetUUID(), open);
    }

	// FIXME: do this automatically, e.g. a ImGuiTreeNodeFlags_AutoCloseChildNodes etc.
	// 노드를 닫는 경우: 1) 모든 하위 노드를 닫고 선택 취소, 2) 하위 노드 중 하나라도 선택된 경우 부모 선택
    static int TreeCloseAndUnselectChildNodes(ActorTreeNode* node, ImGuiSelectionBasicStorage* selection, int depth = 0)
    {
        // Recursive close (the test for depth == 0 is because we call this on a node that was just closed!)
        int unselected_count = selection->Contains(node->GetUUID()) ? 1 : 0;
        if (depth == 0 || TreeNodeGetOpen(node))
        {
            for (ActorTreeNode* child : node->Children)
                unselected_count += TreeCloseAndUnselectChildNodes(child, selection, depth + 1);
            TreeNodeSetOpen(node, false);
        }

        // Select root node if any of its child was selected, otherwise unselect
        selection->SetItemSelected(node->GetUUID(), (depth == 0 && unselected_count > 0));
        return unselected_count;
    }

    // Apply multi-selection requests
    static void ApplySelectionRequests(ImGuiMultiSelectIO* ms_io, ActorTreeNode* tree, ImGuiSelectionBasicStorage* selection)
    {
        for (ImGuiSelectionRequest& req : ms_io->Requests)
        {
            if (req.Type == ImGuiSelectionRequestType_SetAll)
            {
                if (req.Selected)
                    TreeSetAllInOpenNodes(tree, selection, req.Selected);
                else
                    selection->Clear();
            }
            else if (req.Type == ImGuiSelectionRequestType_SetRange)
            {
                //TODO: Fix Selet By Range Logic
                ActorTreeNode* first_node = (ActorTreeNode*)(intptr_t)req.RangeFirstItem;
                ActorTreeNode* last_node = (ActorTreeNode*)(intptr_t)req.RangeLastItem;
                for (ActorTreeNode* node = first_node; node != NULL; node = TreeGetNextNodeInVisibleOrder(node, last_node))
                    selection->SetItemSelected(node->GetUUID(), req.Selected);
            }
        }
    }

    static void TreeSetAllInOpenNodes(ActorTreeNode* node, ImGuiSelectionBasicStorage* selection, bool selected)
    {
        if (node->Parent != NULL) // Root node isn't visible nor selectable in our scheme
            selection->SetItemSelected(node->GetUUID(), selected);
        if (node->Parent == NULL || TreeNodeGetOpen(node))
            for (ActorTreeNode* child : node->Children)
                TreeSetAllInOpenNodes(child, selection, selected);
    }

    // Interpolate in *user-visible order* AND only *over opened nodes*.
	// If you have a sequential mapping tables (e.g. generated after a filter/search pass) this would be simpler.
	// Here the tricks are that:
	// - we store/maintain ActorTreeNode::IndexInParent which allows implementing a linear iterator easily, without searches, without recursion.
	//   this could be replaced by a search in parent, aka 'int index_in_parent = curr_node->Parent->Childs.find_index(curr_node)'
	//   which would only be called when crossing from child to a parent, aka not too much.
	// - we call SetNextItemStorageID() before our TreeNode() calls with an ID which doesn't relate to UI stack,
	//   making it easier to call TreeNodeGetOpen()/TreeNodeSetOpen() from any location.
    static ActorTreeNode* TreeGetNextNodeInVisibleOrder(ActorTreeNode* curr_node, ActorTreeNode* last_node)
    {
        // Reached last node
        if (curr_node == last_node)
            return nullptr;

        // Recurse into childs. Query storage to tell if the node is open.
        if (curr_node->Children.Len() > 0 && TreeNodeGetOpen(curr_node))
            return curr_node->Children[0];

        // Next sibling, then into our own parent
        while (curr_node->Parent != nullptr)
        {
            if (curr_node->IndexInParent + 1 < curr_node->Parent->Children.Len())
                return curr_node->Parent->Children[curr_node->IndexInParent + 1];
            curr_node = curr_node->Parent;
        }
        return nullptr;
    }
};
