--[[
  Quest 740: [Quest] Decommissioned Butterflies 2
  Level Required: 62
  NPC Start: 117, NPC Complete: 117
  Prerequisites: [739]
  Rewards: EXP=792046
]]

local fsm = require('fsm_engine')

local quest_740 = fsm:new({
    id = 740,
    name = "[Quest] Decommissioned Butterflies 2",
    level_required = 62,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {739},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 740: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 740: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 740: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 37,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 740: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 94,
                    count = 80,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 740: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 95,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 740: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 740: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 740: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 740: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 740: Rewards given")
                self:give_rewards({
                    exp = 792046,
                })
            end,
            transitions = {},
        },
    },
})

return quest_740