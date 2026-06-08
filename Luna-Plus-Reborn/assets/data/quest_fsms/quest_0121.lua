--[[
  Quest 121: Defend the Homeland
  Level Required: 25
  NPC Start: 23, NPC Complete: 23
  Prerequisites: [153]
  Rewards: EXP=17393, Gold=3710
]]

local fsm = require('fsm_engine')

local quest_121 = fsm:new({
    id = 121,
    name = "Defend the Homeland",
    level_required = 25,
    npc_start = 23,
    npc_complete = 23,
    prerequisites = {153},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 121: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 23,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 121: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 121: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 14,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 121: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 45,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 121: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 11,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 121: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 23,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 121: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 121: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 23,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 121: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 121: Rewards given")
                self:give_rewards({
                    exp = 17393,
                    gold = 3710,
                    items = {
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_121