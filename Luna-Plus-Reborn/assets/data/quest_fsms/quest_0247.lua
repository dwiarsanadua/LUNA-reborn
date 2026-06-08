--[[
  Quest 247: The Basilisk Statue
  Level Required: 41
  NPC Start: 66, NPC Complete: 66
  Prerequisites: [246]
  Rewards: EXP=99177, Gold=10000
]]

local fsm = require('fsm_engine')

local quest_247 = fsm:new({
    id = 247,
    name = "The Basilisk Statue",
    level_required = 41,
    npc_start = 66,
    npc_complete = 66,
    prerequisites = {246},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 247: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 66,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 247: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 247: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 38,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 247: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 38,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 247: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 66,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 247: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 247: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 66,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 247: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 247: Rewards given")
                self:give_rewards({
                    exp = 99177,
                    gold = 10000,
                    items = {
                        { item_id = 156, count = 1 },
                        { item_id = 30000431, count = 1 },
                        { item_id = 157, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_247