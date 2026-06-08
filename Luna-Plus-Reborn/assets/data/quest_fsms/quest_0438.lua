--[[
  Quest 438: [Party][Quest]Tripping up Giants
  Level Required: 53
  NPC Start: 118, NPC Complete: 118
  Prerequisites: [437]
  Rewards: EXP=288781
]]

local fsm = require('fsm_engine')

local quest_438 = fsm:new({
    id = 438,
    name = "[Party][Quest]Tripping up Giants",
    level_required = 53,
    npc_start = 118,
    npc_complete = 118,
    prerequisites = {437},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 438: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 118,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 438: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 438: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 286,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 438: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 97,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 438: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 118,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 438: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 438: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 118,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 438: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 438: Rewards given")
                self:give_rewards({
                    exp = 288781,
                })
            end,
            transitions = {},
        },
    },
})

return quest_438