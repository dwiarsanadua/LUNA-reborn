--[[
  Quest 955: New Monsters
  Level Required: 109
  NPC Start: 128, NPC Complete: 128
  Prerequisites: [954]
  Rewards: EXP=17049889, Gold=101632
]]

local fsm = require('fsm_engine')

local quest_955 = fsm:new({
    id = 955,
    name = "New Monsters",
    level_required = 109,
    npc_start = 128,
    npc_complete = 128,
    prerequisites = {954},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 955: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 955: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 955: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 345,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 955: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 955: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 955: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 955: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 955: Rewards given")
                self:give_rewards({
                    exp = 17049889,
                    gold = 101632,
                })
            end,
            transitions = {},
        },
    },
})

return quest_955