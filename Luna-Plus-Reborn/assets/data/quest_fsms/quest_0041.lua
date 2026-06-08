--[[
  Quest 41: Clearing the closet of Skeletons
  Level Required: 50
  NPC Start: 43, NPC Complete: 43
  Rewards: EXP=190259, Gold=14030
]]

local fsm = require('fsm_engine')

local quest_41 = fsm:new({
    id = 41,
    name = "Clearing the closet of Skeletons",
    level_required = 50,
    npc_start = 43,
    npc_complete = 43,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 41: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 43,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 41: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 41: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 13,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 41: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 61,
                    count = 8,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 41: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 62,
                    count = 12,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 41: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 43,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 41: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 41: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 43,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 41: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 41: Rewards given")
                self:give_rewards({
                    exp = 190259,
                    gold = 14030,
                    items = {
                        { item_id = 21000004, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_41