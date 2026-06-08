--[[
  Quest 859: Bounty Hunter
  Level Required: 89
  NPC Start: 125, NPC Complete: 125
  Rewards: EXP=4431094, Gold=57826
]]

local fsm = require('fsm_engine')

local quest_859 = fsm:new({
    id = 859,
    name = "Bounty Hunter",
    level_required = 89,
    npc_start = 125,
    npc_complete = 125,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 859: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 125,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 859: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 859: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 287,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 859: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 283,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 859: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 125,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 859: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 859: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 125,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 859: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 859: Rewards given")
                self:give_rewards({
                    exp = 4431094,
                    gold = 57826,
                })
            end,
            transitions = {},
        },
    },
})

return quest_859