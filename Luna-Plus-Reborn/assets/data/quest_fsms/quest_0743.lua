--[[
  Quest 743: [Quest] Decommissioned Butterfly 5
  Level Required: 65
  NPC Start: 117, NPC Complete: 126
  Prerequisites: [742]
  Rewards: EXP=1028873
]]

local fsm = require('fsm_engine')

local quest_743 = fsm:new({
    id = 743,
    name = "[Quest] Decommissioned Butterfly 5",
    level_required = 65,
    npc_start = 117,
    npc_complete = 126,
    prerequisites = {742},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 743: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 743: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 743: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 65,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 743: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 66,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 743: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 743: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 743: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 743: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 743: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 743: Rewards given")
                self:give_rewards({
                    exp = 1028873,
                    items = {
                        { item_id = 378, count = 1 },
                        { item_id = 378, count = 1 },
                        { item_id = 365, count = 1 },
                        { item_id = 365, count = 1 },
                        { item_id = 365, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_743