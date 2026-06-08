--[[
  Quest 742: [Quest] Decommissioned Butterfly 4
  Level Required: 64
  NPC Start: 117, NPC Complete: 117
  Prerequisites: [741]
  Rewards: EXP=945115
]]

local fsm = require('fsm_engine')

local quest_742 = fsm:new({
    id = 742,
    name = "[Quest] Decommissioned Butterfly 4",
    level_required = 64,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {741},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 742: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 742: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 742: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 253,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 742: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 78,
                    count = 80,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 742: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 79,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 742: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 742: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 742: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 742: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 742: Rewards given")
                self:give_rewards({
                    exp = 945115,
                })
            end,
            transitions = {},
        },
    },
})

return quest_742