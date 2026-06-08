--[[
  Quest 131: Securing a Trade Route
  Level Required: 3
  NPC Start: 50, NPC Complete: 50
  Prerequisites: [130]
  Rewards: EXP=44, Gold=700
]]

local fsm = require('fsm_engine')

local quest_131 = fsm:new({
    id = 131,
    name = "Securing a Trade Route",
    level_required = 3,
    npc_start = 50,
    npc_complete = 50,
    prerequisites = {130},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 131: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 50,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 131: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 131: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 21,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 131: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 25,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 131: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 23,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 131: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 131: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 131: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 50,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 131: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 131: Rewards given")
                self:give_rewards({
                    exp = 44,
                    gold = 700,
                    items = {
                        { item_id = 21000001, count = 10 },
                        { item_id = 21000006, count = 8 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_131