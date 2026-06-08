--[[
  Quest 166: Orc Alliance
  Level Required: 36
  NPC Start: 33, NPC Complete: 33
  Prerequisites: [165]
  Rewards: EXP=62194, Gold=6700
]]

local fsm = require('fsm_engine')

local quest_166 = fsm:new({
    id = 166,
    name = "Orc Alliance",
    level_required = 36,
    npc_start = 33,
    npc_complete = 33,
    prerequisites = {165},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 166: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 166: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 166: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 57,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 166: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 58,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 166: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 32,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 166: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 57,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 166: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 166: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 166: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 166: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 166: Rewards given")
                self:give_rewards({
                    exp = 62194,
                    gold = 6700,
                    items = {
                        { item_id = 30000187, count = 2 },
                        { item_id = 21000013, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_166