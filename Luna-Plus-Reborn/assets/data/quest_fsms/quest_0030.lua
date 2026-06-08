--[[
  Quest 30: Turning the Tide
  Level Required: 35
  NPC Start: 31, NPC Complete: 31
  Rewards: EXP=55812, Gold=4280
]]

local fsm = require('fsm_engine')

local quest_30 = fsm:new({
    id = 30,
    name = "Turning the Tide",
    level_required = 35,
    npc_start = 31,
    npc_complete = 31,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 30: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 31,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 30: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 30: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 51,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 30: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 56,
                    count = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 30: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 31,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 30: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 30: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 31,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 30: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 30: Rewards given")
                self:give_rewards({
                    exp = 55812,
                    gold = 4280,
                    items = {
                        { item_id = 30000825, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_30