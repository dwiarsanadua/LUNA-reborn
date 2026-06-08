--[[
  Quest 444: [Party][Quest] The wild ground
  Level Required: 54
  NPC Start: 119, NPC Complete: 119
  Prerequisites: [443]
  Rewards: EXP=315085, Gold=225078
]]

local fsm = require('fsm_engine')

local quest_444 = fsm:new({
    id = 444,
    name = "[Party][Quest] The wild ground",
    level_required = 54,
    npc_start = 119,
    npc_complete = 119,
    prerequisites = {443},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 444: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 444: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 444: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 94,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 444: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 95,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 444: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 444: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 444: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 444: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 444: Rewards given")
                self:give_rewards({
                    exp = 315085,
                    gold = 225078,
                })
            end,
            transitions = {},
        },
    },
})

return quest_444