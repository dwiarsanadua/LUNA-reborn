--[[
  Quest 929: [Party][Quest] Pirate Fighter Subjugation.
  Level Required: 0
  NPC Start: 537, NPC Complete: 537
  Prerequisites: [928]
  Rewards: EXP=11447014, Gold=81769
]]

local fsm = require('fsm_engine')

local quest_929 = fsm:new({
    id = 929,
    name = "[Party][Quest] Pirate Fighter Subjugation.",
    level_required = 0,
    npc_start = 537,
    npc_complete = 537,
    prerequisites = {928},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 929: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 929: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 929: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 295,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 929: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 296,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 929: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 929: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 929: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 929: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 929: Rewards given")
                self:give_rewards({
                    exp = 11447014,
                    gold = 81769,
                })
            end,
            transitions = {},
        },
    },
})

return quest_929