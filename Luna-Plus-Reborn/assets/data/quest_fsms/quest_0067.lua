--[[
  Quest 67: Battle Support
  Level Required: 3
  NPC Start: 49, NPC Complete: 49
  Rewards: EXP=44, Gold=930
]]

local fsm = require('fsm_engine')

local quest_67 = fsm:new({
    id = 67,
    name = "Battle Support",
    level_required = 3,
    npc_start = 49,
    npc_complete = 49,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 67: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 49,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 67: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 67: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 21,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 67: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 25,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 67: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 49,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 67: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 67: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 49,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 67: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 67: Rewards given")
                self:give_rewards({
                    exp = 44,
                    gold = 930,
                    items = {
                        { item_id = 21000006, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_67