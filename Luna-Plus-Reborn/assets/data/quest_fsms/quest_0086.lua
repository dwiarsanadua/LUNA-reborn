--[[
  Quest 86: Protecting the Patrol Party
  Level Required: 13
  NPC Start: 13, NPC Complete: 49
  Rewards: EXP=1982, Gold=1305
]]

local fsm = require('fsm_engine')

local quest_86 = fsm:new({
    id = 86,
    name = "Protecting the Patrol Party",
    level_required = 13,
    npc_start = 13,
    npc_complete = 49,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 86: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 86: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 86: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 29,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 86: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 31,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 86: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 86: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 86: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 49,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 86: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 86: Rewards given")
                self:give_rewards({
                    exp = 1982,
                    gold = 1305,
                    items = {
                        { item_id = 21000007, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_86