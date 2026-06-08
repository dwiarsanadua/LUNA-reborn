--[[
  Quest 179: The Counterattack Begins
  Level Required: 38
  NPC Start: 31, NPC Complete: 37
  Prerequisites: [178]
  Rewards: EXP=96964, Gold=6870
]]

local fsm = require('fsm_engine')

local quest_179 = fsm:new({
    id = 179,
    name = "The Counterattack Begins",
    level_required = 38,
    npc_start = 31,
    npc_complete = 37,
    prerequisites = {178},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 179: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 31,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 179: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 179: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 58,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 179: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 32,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 179: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 31,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 179: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 37,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 179: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 179: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 37,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 179: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 179: Rewards given")
                self:give_rewards({
                    exp = 96964,
                    gold = 6870,
                    items = {
                        { item_id = 21001505, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_179