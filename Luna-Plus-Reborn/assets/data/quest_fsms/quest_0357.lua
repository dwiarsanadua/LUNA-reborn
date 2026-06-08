--[[
  Quest 357: [Party] The Final Blow
  Level Required: 42
  NPC Start: 59, NPC Complete: 59
  Prerequisites: [356]
  Rewards: EXP=114742, Gold=15000
]]

local fsm = require('fsm_engine')

local quest_357 = fsm:new({
    id = 357,
    name = "[Party] The Final Blow",
    level_required = 42,
    npc_start = 59,
    npc_complete = 59,
    prerequisites = {356},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 357: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 357: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 357: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 162,
                    count = 70,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 357: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 163,
                    count = 70,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 357: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 357: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 357: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 357: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 357: Rewards given")
                self:give_rewards({
                    exp = 114742,
                    gold = 15000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_357