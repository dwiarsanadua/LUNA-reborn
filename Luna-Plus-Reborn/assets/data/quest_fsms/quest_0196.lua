--[[
  Quest 196: Merchant Group Crisis
  Level Required: 43
  NPC Start: 3, NPC Complete: 62
  Prerequisites: [186]
  Rewards: EXP=118872, Gold=4500
]]

local fsm = require('fsm_engine')

local quest_196 = fsm:new({
    id = 196,
    name = "Merchant Group Crisis",
    level_required = 43,
    npc_start = 3,
    npc_complete = 62,
    prerequisites = {186},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 196: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 3,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 196: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 196: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 38,
                    count = 35,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 196: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 196: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 196: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 196: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 196: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 196: Rewards given")
                self:give_rewards({
                    exp = 118872,
                    gold = 4500,
                })
            end,
            transitions = {},
        },
    },
})

return quest_196