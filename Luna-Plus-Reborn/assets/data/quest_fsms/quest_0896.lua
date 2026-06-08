--[[
  Quest 896: Swatting Mosquitos
  Level Required: 98
  NPC Start: 540, NPC Complete: 541
  Rewards: EXP=8938143, Gold=77721
]]

local fsm = require('fsm_engine')

local quest_896 = fsm:new({
    id = 896,
    name = "Swatting Mosquitos",
    level_required = 98,
    npc_start = 540,
    npc_complete = 541,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 896: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 896: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 896: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 320,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 896: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 896: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 896: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 896: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 896: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 896: Rewards given")
                self:give_rewards({
                    exp = 8938143,
                    gold = 77721,
                    items = {
                        { item_id = 30000686, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_896